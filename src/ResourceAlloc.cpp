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

 // --------------------------------------------------------------------------
 // Authors and contributors to this file:
 // Jens Joschinski (IBM);
 // --------------------------------------------------------------------------

#include "ResourceAlloc.h"

/** @cond */
#include "nlohmann/json.hpp"
#include "easylogging++.h"
/** @endcond */

ResourceAlloc::ResourceAlloc(const nlohmann::json& traits) {
    if(!traits.is_object()){
        throw std::invalid_argument("Resource alloc json is broken or empty");}
    try {
        conversionEfficiency = traits.at("conversionEfficiency");
        maintenanceCosts = traits.at("maintenanceCosts");
        seedAllocation = traits.at("seedAllocation");
        biomassAllocation = traits.at("biomassAllocation");
        maxBiomass = traits.at("maxBiomass");
        check();
    } catch (nlohmann::json::exception& e) {
        LOG(DEBUG) << "fields in resource alloc file are: " << traits.dump(4);
        LOG(ERROR) << "Exception when reading from json ResourceAlloc: " << e.what() << '\n';
        throw;
    } catch (std::invalid_argument& e) {
        LOG(ERROR) << "Invalid argument when initializing ResourceAlloc: " << e.what() << '\n';
        throw;
    }

}


void ResourceAlloc::check() {
    if (conversionEfficiency < 0.0    || conversionEfficiency > 1.0 ||
        maintenanceCosts < 0.0        || maintenanceCosts > 1.0     ||
        seedAllocation < 0.0          || seedAllocation > 1.0       ||
        biomassAllocation < 0.0       || biomassAllocation > 1.0    || 
        maxBiomass < 0              ){
        throw std::invalid_argument("Invalid values for ResourceAlloc traits");
    }

    if (seedAllocation + biomassAllocation + maintenanceCosts > 1.0) {
        throw std::invalid_argument("Seed, biomass  and maintenance allocations may not be larger than 1");
    }
}