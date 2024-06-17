/*------------------------------------------------------------------------------
Copyright (C)  2022 - present  Studio Animal-Aided Design

This file is part of the INDIVIDUAL-BASED PLANT MODEL.

INDIVIDUAL-BASED PLANT MODEL is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your option) any later version.

INDIVIDUAL-BASED PLANT MODEL is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with INDIVIDUAL-BASED PLANT MODEL.
If not, see <https://www.gnu.org/licenses/>. */

// --------------------------------------------------------------------------
 /* INDIVIDUAL-BASED PLANT MODEL is derived from the ECOLOPES PLANT MODEL, Copyright (C) 2022-present Studio Animal-Aided Design.

 * ECOLOPES PLANT MODEL is derived, modified and extended from FATE, https://github.com/leca-dev/RFate.git/ 
 * Copyright (C) 2021 Isabelle Boulangeat, Damien Georges, Maya Guéguen, Wilfried Thuiller 
 * For contributions to this particular file, see section "Authors and contributors".*/
// --------------------------------------------------------------------------

 // ----------------------------------------------------------------------------
 // Authors and contributors to this file:
 // Jens Joschinski (IBM)
 // ----------------------------------------------------------------------------

#include "Community.h"
#include "SeedPool.h"
#include "Individual.h"
#include "Soil.h"
#include "Illumination.h"
#include "rng.h"

/** @cond */
#include <memory>
#include <map>
#include <string>
#include <string_view>
#include <vector>
#include <cassert>
#include <algorithm>
/** @endcond */


Community::Community(std::weak_ptr<Soil> soil, 
        std::map<std::string_view, const Traits*> traits, int maxIndividuals, int initialSeeds):
        m_soil(soil){
    assert(!soil.expired());
    assert( maxIndividuals > 0);
    assert (traits.size() > 0);
    assert (initialSeeds >= 0);
    int individuals = 0;
    std::vector<std::string_view> suitableTypes;
    for (auto& trait : traits){
        if (HabSuit::wouldBeSuitable(trait.second->soilReqs, soil)){
            suitableTypes.push_back(trait.first);
            m_traits.insert({trait.first, trait.second});
        }
    }
    while (suitableTypes.size() >0 && individuals < maxIndividuals){
        std::string_view traitName = suitableTypes.at(rand() % suitableTypes.size());
        if (createIndividualAndSeedPool(m_traits.at(traitName), traitName, initialSeeds)){
            individuals++;
        } else {
            suitableTypes.erase(std::remove(suitableTypes.begin(), suitableTypes.end(), traitName), suitableTypes.end());
        }
    }
}

Community::Community(std::weak_ptr<Soil> soil, 
        std::map<std::string_view, const Traits*> traits, int maxIndividuals, int initialSeeds,
        const nlohmann::json& individuals):
        m_soil(soil){
    assert(!soil.expired());
    assert(maxIndividuals > 0);
    assert (traits.size() > 0);
    assert (initialSeeds >= 0);
    assert(individuals.size() > 0 && individuals.size() <= maxIndividuals);
    for (auto it = individuals.begin(); it != individuals.end(); ++it){
        if (traits.find(it.key()) == traits.end()){
            throw std::invalid_argument("No traits found for individual " + it.key() +  " in json file");
        }
        if (!traits.count(it.key()) || !HabSuit::wouldBeSuitable(traits.at(it.key())->soilReqs, m_soil)){
            throw std::invalid_argument("Individuals in json file are not suitable for the soil");
        } else {
            std::unique_ptr<Individual> ind = Individual::create(traits.at(it.key()), m_soil, it.value());
            assert(ind != nullptr);
            m_individuals.insert({it.key(), std::move(ind)});
            if (!m_seeds.count(it.key())) {
                std::unique_ptr<SeedPool> sp = std::make_unique<SeedPool>(traits.at(it.key()), 100 * m_soil.lock()->m_capacity);
                sp->increase(initialSeeds);
                m_seeds.insert({it.key(), std::move(sp)});
            }
        }
    }
}

bool Community::createIndividualAndSeedPool(const Traits* traits, std::string_view name, int initialSeeds){
    std::unique_ptr<Individual> ind = Individual::create(traits, m_soil); 
    if (ind == nullptr) return false;
    else {
        m_individuals.insert({name, std::move(ind)});
        if (!m_seeds.count(name)) {
            std::unique_ptr<SeedPool> sp = std::make_unique<SeedPool>(traits, 100 * m_soil.lock()->m_capacity);
            sp->increase(initialSeeds);
            m_seeds.insert({name, std::move(sp)});
        }
        return true;
    }
}

Community::~Community(){
    m_individuals.clear();
    m_seeds.clear();
    m_traits.clear();
    m_soil.reset(); //Must be last, because ~Individual would complain if soil is already deleted
}

void Community::rainSeeds(const std::map<std::string_view, int>& seeds) {
    for (const auto& name : seeds) {
        if (m_traits.find(name.first) == m_traits.end()) {//m_traits contains names of all suitable species
            continue;
        } else if (m_seeds.find(name.first) == m_seeds.end()){ //suitable but not yet present
            int number = std::min(name.second, 100 * m_soil.lock()->m_capacity);
            std::unique_ptr<SeedPool> sp = std::make_unique<SeedPool>(m_traits.at(name.first), number);
            m_seeds.insert({name.first, std::move(sp)});
        }else{
            m_seeds.at(name.first)->increase(name.second);
        }
    }
}

int Community::getCount(std::string_view type) const {
    if (type == "") return m_individuals.size();
    else return m_individuals.count(type);
}

int Community::getCount(int from, int to, std::string_view type) const {
    int count = 0;
    auto range = (type == "") ? 
                 std::make_pair(m_individuals.begin(), m_individuals.end()) : 
                 m_individuals.equal_range(type);
    for (auto i = range.first; i != range.second; ++i) {
        if (i->second->getArea(from, to) > 0.0) {
            ++count;
        }
    }
    return count;
}

float Community::getBiomass(std::string_view type) const{
    float biomass = 0.0;
    auto range = (type == "") ? 
                 std::make_pair(m_individuals.begin(), m_individuals.end()) : 
                 m_individuals.equal_range(type);
    for (auto i = range.first; i != range.second; ++i) {
        biomass += i->second->getBiomass(true);
    }
    return biomass;

}

std::map<std::string, int> Community::age(){
    std::map<std::string, int> seedsProduced;
    auto it = m_individuals.begin();
    while (it != m_individuals.end()){
        std::string name = std::string(it->first);
        if (seedsProduced.find(name) == seedsProduced.end()) seedsProduced[name] = 0;
        std::pair<bool, int> res = it->second->age(); //res = survival and seeds produced
        if (!res.first){it = m_individuals.erase(it);}
        else ++it;
        seedsProduced[name] += res.second; //independent of surivival. Plant may be killed after producing seeds
    }
    recruit();
    return seedsProduced;
}



void Community::recruit(){
    std::vector<std::pair<std::string_view, SeedPool*>> seedPoolInRandomOrder;
    for (auto& [seedType, seedPool] : m_seeds){
        int germinantsToAdd = seedPool->age(0);
        for (int i = 0; i < germinantsToAdd; i++) {
            seedPoolInRandomOrder.emplace_back(seedType, seedPool.get());
        }
    }
    std::shuffle(seedPoolInRandomOrder.begin(), seedPoolInRandomOrder.end(), RNGs::mersenne);

   for (const auto& [germinantType, germinantPool] : seedPoolInRandomOrder){
        std::unique_ptr<Individual> ind = Individual::create(m_traits.at(germinantType), m_soil);
        if (ind == nullptr) continue;
        m_individuals.insert({germinantType, std::move(ind)});
    }
}

std::vector<std::weak_ptr<Individual>> Community::getIndividuals() const{
    std::vector<std::weak_ptr <Individual>> out;
    for (auto& [name, ind] : m_individuals){ out.push_back(ind);}
    return out;
}