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
#include <cmath>
#include <nlohmann/json.hpp>
/** @endcond */


PlantResource::PlantResource(const ResourceAlloc* const resAlloc_ptr): m_resAlloc_ptr(resAlloc_ptr), resources(0.0), biomass(5.0){
    #ifdef TESTING
    biomass = 100.0;
    #endif
}
PlantResource::PlantResource(const ResourceAlloc* const resAlloc_ptr, nlohmann::json j): m_resAlloc_ptr(resAlloc_ptr){
    assert(j.size() > 0);
    assert(m_resAlloc_ptr != nullptr);
    try{
        resources = j.at("resources");
        biomass = j.at("biomass");
    }catch(std::out_of_range& e){
        throw std::invalid_argument("PlantResource: json does not contain resources or biomass");
    }
    catch(nlohmann::json::type_error& e){
        throw std::invalid_argument("PlantResource: resources must be of type float, biomass must be of type float");
    }
    check();
}

void PlantResource::check() const{
    if (biomass < 0.0 || resources < 0.0) throw std::invalid_argument("PlantResource: biomass and resources must be positive: " + std::to_string(biomass) + ", " + std::to_string(resources));
    if (biomass > m_resAlloc_ptr->maxBiomass) throw std::invalid_argument("PlantResource: biomass must not exceed maximum biomass: " + std::to_string(m_resAlloc_ptr->maxBiomass));
}



PlantResource::~PlantResource(){}

void PlantResource::updateResource(int light, bool soilIsSuitable){
    assert (light >= 0);
    if (soilIsSuitable){
    resources += light * m_resAlloc_ptr->conversionEfficiency;
    }
}

std::pair<bool, int> PlantResource::allocateResources(){
    assert(m_resAlloc_ptr != nullptr);
    assert(m_resAlloc_ptr->seedAllocation >= 0.0 && m_resAlloc_ptr->seedAllocation <= 1.0);
    assert(m_resAlloc_ptr->biomassAllocation >= 0.0 && m_resAlloc_ptr->biomassAllocation <= 1.0);

    resources -= static_cast<int> (biomass * m_resAlloc_ptr->maintenanceCosts);
    if (resources <= 0.0){
        return std::make_pair(false, 0);
    }
    else {
        int seeds = static_cast<int> (resources * m_resAlloc_ptr->seedAllocation); //round down
        resources -= seeds;

        double potentialGrowth = resources * m_resAlloc_ptr->biomassAllocation;
        assert ( m_resAlloc_ptr->maxBiomass >= biomass);
        double maxGrowth = sqrt(m_resAlloc_ptr->maxBiomass - biomass);
        float addBiomass =  std::min(std::min(potentialGrowth, maxGrowth), static_cast<double> (m_resAlloc_ptr->maxBiomass - biomass));
        biomass += addBiomass;
        resources -= addBiomass;
        assert (biomass >= 0.0 && biomass <= m_resAlloc_ptr->maxBiomass);
        assert(resources >= 0.0);
        return std::make_pair(true, seeds);
    }
}

float PlantResource::disturb(float amount){
    assert(amount >= 0.0);
    biomass -= amount;
    if (biomass < 0.0) {
        int deficit(-biomass);
        biomass = 0.0;
        return deficit;
    }
    return 0.0;
}

float PlantResource::depleteResources(float amount){
    assert(amount >= 0.0);
    resources -= amount;
    if (resources < 0.0) {
        float deficit(-resources);
        resources = 0.0;
        return deficit;
    }
    return 0.0;
}

float PlantResource::getBiomass() const{
 return biomass;
}