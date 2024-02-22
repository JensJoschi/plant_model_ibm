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
 // Jens Joschinski (IBM); rewrite of PFG class (RFATE/EPM)
 // --------------------------------------------------------------------------

#include "SoilRequirements.h"

/** @cond */
#include "nlohmann/json.hpp"
#include "easylogging++.h"
/** @endcond */


SoilRequirements::SoilRequirements(const nlohmann::json& SoilReqTraits) {
    try {
        if (SoilReqTraits.at("minDepth").is_number_integer()) {
            minDepth = SoilReqTraits.at("minDepth").get<int>();
        } else {
            throw std::runtime_error("type of minDepth must be integer, but is " + 
            std::string(SoilReqTraits.at("minDepth").type_name()));
        }
        acceptedSoils = SoilReqTraits.at("acceptedSoils").get<std::map<std::string, bool>>();
        check();
    } catch (nlohmann::json::exception& e) {
        LOG(DEBUG) << "fields in Soil Req file are: " << SoilReqTraits.dump(4);
        LOG(ERROR) << "Exception when initializing SoilRequirements: " << e.what() << '\n';
        throw;
    } catch (std::invalid_argument& e) {
        LOG(ERROR) << "Invalid argument when initializing SoilRequirements: " << e.what() << '\n';
        throw;
    }
}

void SoilRequirements::check() {
    // Check if the values are within an acceptable range
    if (minDepth < 0) {
        throw std::invalid_argument("SoilRequirements parameters must be positive");
    }
    if (acceptedSoils.empty()) {
        throw std::invalid_argument("SoilRequirements must accept at least one soil type");
    }
}