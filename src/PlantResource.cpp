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




#include "PlantResource.h"
#include "ResourceAlloc.h"

/** @cond */
#include <cassert>
#include <utility>
/** @endcond */

PlantResource::PlantResource(const ResourceAlloc* const resAlloc_ptr): m_resAlloc_ptr(resAlloc_ptr), resources(0), biomass(100){}
PlantResource::~PlantResource(){}

void PlantResource::updateResource(int light, bool soilIsSuitable){
    assert (light >= 0);
    if (soilIsSuitable){
    resources += light * m_resAlloc_ptr->conversionEfficiency;
    }
}

std::pair<bool, int> PlantResource::allocateResources(){
    assert(biomass * m_resAlloc_ptr->maxInvestment >= 1); //small initial biomass may mean that plant is never able to grow (rounding/int conversion and low maxInvest)
    assert(m_resAlloc_ptr != nullptr);
    assert(m_resAlloc_ptr->seedAllocation >= 0 && m_resAlloc_ptr->seedAllocation <= 1);
    assert(m_resAlloc_ptr->biomassAllocation >= 0 && m_resAlloc_ptr->biomassAllocation <= 1);

    resources -= static_cast<int> (biomass * m_resAlloc_ptr->maintenanceCosts);
    if (resources <= 0){
        return std::make_pair(false, 0);
    }
    else {
        int seeds = static_cast<int> (resources * m_resAlloc_ptr->seedAllocation);
        resources -= seeds;
        int addBiomass = static_cast<int> (std::min(resources * m_resAlloc_ptr->biomassAllocation , biomass * m_resAlloc_ptr->maxInvestment));
        biomass += addBiomass;
        resources -= addBiomass;
        return std::make_pair(true, seeds);
    }
}

int PlantResource::disturb(int amount){
    assert(amount >= 0);
    biomass -= amount;
    if (biomass < 0) {
        int deficit(-biomass);
        biomass = 0;
        return deficit;
    }
    return 0;
}

int PlantResource::depleteResources(int amount){
    assert(amount >= 0);
    resources -= amount;
    if (resources < 0) {
        int deficit(-resources);
        resources = 0;
        return deficit;
    }
    return 0;
}

int PlantResource::getBiomass(bool shadingCorrected) const{
    if (shadingCorrected) return biomass * m_resAlloc_ptr->shadeFactor;
    else return biomass;
}