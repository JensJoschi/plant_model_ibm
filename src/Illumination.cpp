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
 
#include "Illumination.h"
#include "Voxel.h"
#include "Individual.h"
#include "rng.h"
#ifdef min
    #undef min
#endif
/** @cond */
#include <vector>
#include <map>
#include <algorithm>
/** @endcond */

Illumination::Illumination( const std::vector<Stratum>& strata, int lightDistributionFactor, float diffusion): 
    strata(strata), lightDistributionFactor(lightDistributionFactor), diffusion(diffusion) {
        assert(diffusion >= 0.0f && diffusion <= 1.0f);
        assert (lightDistributionFactor > 0);
    }

void Illumination::distributeIndividuals(Community& comm){
    //individual areas will be added to surfaceAreas. This is a good time to clean up surfaceAreas, as some neighbours may have died,
    //but still exist here
    for (auto& [stratum, individuals] : surfaceAreas){
        individuals.erase(std::remove_if(individuals.begin(), individuals.end(),
                 [](const IndivArea_t& i) { return i.first.expired(); }),
                individuals.end());
    }
    std::vector<std::weak_ptr<Individual>> i = comm.getIndividuals(); //these , on the other hand, should not be expired!
    for (auto& stratum : strata){
        for (auto& ind : i){
            assert(!ind.expired());
            std::shared_ptr<Individual> currIndividual = ind.lock();
            float plantArea = currIndividual->getArea(stratum.from, stratum.to);
            if (plantArea>0.0f){
                IndivArea_t indArea = {currIndividual, plantArea};
                addHereOrElsewhere(indArea, stratum);
            }
        }
    }
}

void Illumination::sendLightBeam(int light){
    assert(light >= 0);
    for (int i = strata.size()-1; i >= 0; --i){
        choosePlants(strata[i]);
        light = distributeLightToIndividuals(light, strata[i]);
    }
}

void Illumination::addNeighbours(const std::vector<Illumination*>& n_illumin){
    for (auto& ill : n_illumin){
        neighbours.push_back(ill);
    }
}

float Illumination::getPlantCover(bool includeDead) const{
    float totalArea = 0.0f;
    for (auto& [stratum, individuals] : surfaceAreas){
        for (auto& [individual, area] : individuals){
            if (includeDead || !individual.expired())  totalArea += area;
        }
    }
    return totalArea;
}

void Illumination::clearIndividuals(){
    for (auto& [stratum, individuals] : surfaceAreas){
        individuals.clear();
    }
}

//--PRIVATE FUNCTIONS--//

void Illumination::addHereOrElsewhere(IndivArea_t individual, const Stratum& stratum){
    //this recursive function may loop over the neighbour's neighbours, thus returing back to cells.
    //to prevent this, check that the individual is not already in the surfaceAreas
    auto it = std::find_if(surfaceAreas[stratum].begin(), surfaceAreas[stratum].end(),[&individual](const IndivArea_t& elem) {
            assert (!elem.first.expired());
            assert (!individual.first.expired());
            return elem.first.lock() == individual.first.lock();
        }
    );
    if (it != surfaceAreas[stratum].end()){
        return;
    }

    if (individual.second <= stratum.area){
        this->surfaceAreas[stratum].emplace_back(individual);
    } else {
        IndivArea_t amountThatFits = {individual.first, stratum.area};
        this->surfaceAreas[stratum].emplace_back(amountThatFits);
        assert(neighbours.size() > 0);
        IndivArea_t overflow = {individual.first, (individual.second - stratum.area)/neighbours.size()};
        for (auto& n : neighbours){
            n->addHereOrElsewhere(overflow, stratum);
        }
    }
}


void Illumination::choosePlants(const Stratum& stratum){
    if (!surfaceAreas.count(stratum)) return;
    float totalArea = 0.0f;
    surfaceAreas[stratum].erase(std::remove_if(surfaceAreas[stratum].begin(), surfaceAreas[stratum].end(),
                 [](const IndivArea_t& i) { return i.first.expired(); }),
                surfaceAreas[stratum].end());
    auto& areaS = surfaceAreas[stratum];

    for (auto& i : areaS){
        assert(i.second >= 0.0);
        totalArea += i.second;
    }
    float reduceBy = (totalArea - stratum.area)/lightDistributionFactor;
    float tolerance = 0.001f; //to deal with floating point precision errors
    while ((totalArea - stratum.area) > tolerance){
        std::uniform_int_distribution<> distrib(0, areaS.size() - 1);
        int index = distrib(RNGs::mersenne);
        auto chosen = std::next(areaS.begin(), index);
        assert (chosen->second >= 0.0f && !chosen->first.expired());
        float area = chosen->second;

        if (area > reduceBy){
            chosen->second -= reduceBy;
            totalArea -= reduceBy;
        } else {
            totalArea -= area;
            areaS.erase(chosen);
        }
    }
}

int Illumination::distributeLightToIndividuals(int light, const Stratum& stratum){
    assert(light >= 0);
    assert(stratum.from >= 0.0f && stratum.to > stratum.from);
    assert(stratum.area >= 0.0f);
    float totalArea = 0.0f;
    for (auto& i : surfaceAreas[stratum]) totalArea += i.second;
    float tolerance = 0.001f; //to deal with floating point precision errors
    assert(totalArea <= (stratum.area + tolerance) && totalArea >= 0.0f);
    if (totalArea > stratum.area + tolerance) totalArea = stratum.area;
    if (totalArea == 0.0f) return light;

    float amountThatHitsPlants = std::min (light * (totalArea/stratum.area), static_cast<float> (light));
    if (stratum.from != 0) amountThatHitsPlants = amountThatHitsPlants * (1.0-diffusion);
    assert(amountThatHitsPlants <= light);
 
    for (auto& [individual, area] : surfaceAreas[stratum]){
        assert(!individual.expired());
        float share = area/totalArea;
        int resources = std::round (share * amountThatHitsPlants);
        assert(resources>=0);
       individual.lock()->feed(resources);
    }
    return light - amountThatHitsPlants;
}



