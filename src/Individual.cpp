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

#include "Individual.h"
#include "Traits.h"
#include "ResourceAlloc.h"
#include "LifeHistory.h"
#include "SoilRequirements.h"

#include <iostream>

Individual::Individual(const Traits* traits, std::shared_ptr<Soil> soil): 
        m_LifeHist_ptr(traits->lifeHist),m_SoilReq_ptr(traits->soilReqs), m_resPool_ptr(std::make_unique<PlantResource>(traits->allocation)),
        m_age(0), m_height(0.0), m_biomass(0), m_soil_ptr(soil){
    assert(m_LifeHist_ptr);
    assert(m_SoilReq_ptr);
    assert(m_soil_ptr);
    assert(m_resPool_ptr);
}

Individual::~Individual(){
    m_resPool_ptr = nullptr;
}

void Individual::feed(const int light){
    assert(light >=0);
    m_resPool_ptr->updateResource(light);
};

int Individual::disturb(int amount){
    assert(amount >= 0);
    m_biomass -= amount;
    if (m_biomass < 0) {
        int deficit(-m_biomass);
        m_biomass = 0;
        return deficit;
    }
    return 0;
}

int Individual::disturb(int amount, int from, int to){
    assert(amount >= 0);
    assert(from >= 0);
    assert(to <= m_height);
    assert(to >= from);
    int biomass = getBiomass(from, to);
    m_biomass -= std::min(amount, biomass);
    int deficit = amount > biomass ? amount - biomass : 0; 
    return deficit;
}

int Individual::depleteResources(int amount){
    assert(amount >= 0);
    std::cerr << "calling unimplemented function Individual::depleteResources";
    abort();
    // return m_resPool_ptr->removeResource(amount);
}

bool Individual::doesItDie() const{
    return (m_age > m_LifeHist_ptr->L || m_resPool_ptr->isResourceCritical());
}

int Individual::getBiomass() const{
    return m_biomass;
}

int Individual::getBiomass(int from, int to) const{
    assert(from >= 0);
    assert(to <= m_height);
    assert(to >= from);
    return m_biomass * (to - from) / m_height;
}

int Individual::getShading(int from, int to) const{
    return m_LifeHist_ptr->ShadeFactor * getBiomass(from, to);
}

int Individual::getHeight() const{
    return m_height;
}

int Individual::age(){
    m_age ++;
    double x0 = m_LifeHist_ptr->M / 2.0;
    double k = 1;
    m_height = m_LifeHist_ptr->HMax / (1 + exp(-k * (m_age - x0)));
    int seeds = useResources();
    return seeds;
}


//====================PRIVATE========================
int Individual::useResources(){   
    Allocations toDistribute = m_resPool_ptr->allocateResources(m_biomass);
    m_biomass += toDistribute.biomass;
    return toDistribute.seeds;
}