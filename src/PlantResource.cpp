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
 // Jens Joschinski
 // ----------------------------------------------------------------------------

 // ----------------------------------------------------------------------------
 //THIS FILE IS FOR A NEW IMPLEMENTATION OF PLANT MODEL AND NOT BEING USED YET.
 // ----------------------------------------------------------------------------


#include "PlantResource.h"
#include "ResourceAlloc.h"

/** @cond */
#include <cassert>
/** @endcond */

PlantResource::PlantResource(const ResourceAlloc* const resAlloc_ptr): m_resAlloc_ptr(resAlloc_ptr), resources(0){}
PlantResource::~PlantResource(){}

int PlantResource::updateResource(const int light){
    assert (light >= 0);
    resources += light * m_resAlloc_ptr->conversionEfficiency;
    return resources;
}

bool PlantResource::isResourceCritical() const{
    return resources <= 0;
}

Allocations PlantResource::allocateResources(int biomass){
    assert(biomass * m_resAlloc_ptr->maxInvestment >= 1); // plant growth rate. 1 * 0.05 would make 0.05 new biomass, but because biomass is type int, this would be rounded to 0.
    assert(m_resAlloc_ptr != nullptr);
    assert(m_resAlloc_ptr->seedAllocation >= 0 && m_resAlloc_ptr->seedAllocation <= 1);
    assert(m_resAlloc_ptr->biomassAllocation >= 0 && m_resAlloc_ptr->biomassAllocation <= 1);

    Allocations alloc;
    alloc.seeds = 0;
    alloc.biomass = 0;
    resources -= static_cast<int> (biomass * m_resAlloc_ptr->maintenanceCosts);
    if (resources >0){
        alloc.seeds = static_cast<int> (resources * m_resAlloc_ptr->seedAllocation);
        alloc.biomass = static_cast<int> (std::min(resources * m_resAlloc_ptr->biomassAllocation , biomass * m_resAlloc_ptr->maxInvestment));
        resources -= (alloc.seeds + alloc.biomass);
    }
    return alloc;
}
