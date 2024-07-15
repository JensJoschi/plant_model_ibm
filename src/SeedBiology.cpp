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

#include "SeedBiology.h"

/** @cond */
#include "nlohmann/json.hpp"
#include "easylogging++.h"
/** @endcond */

SeedBiology::SeedBiology(const nlohmann::json& traits) {
    if(!traits.is_object()){
        throw std::invalid_argument("SeedBiology json is broken or empty");}
    try {
        dormancy = traits.at("Dormancy");
        germinationSuccess = traits.at("GerminationSuccess");
        mortalityDormant = traits.at("MortalityDormant");
        dormancyBreakRate = traits.at("DormancyBreakRate");
        check();
    } catch (nlohmann::json::exception& e) {
        LOG(DEBUG) << "fields in file Seed Biology file are: " << traits.dump(4);
        LOG(ERROR) << "Exception when initializing SeedBiology: " << e.what() << '\n';
        throw;
    } catch (std::invalid_argument& e) {
        LOG(ERROR) << "Invalid argument when initializing SeedBiology: " << e.what() << '\n';
        throw;
    }
}

void SeedBiology::check() {
    // Check if the values are within an acceptable range
    if (germinationSuccess < 0 || mortalityDormant < 0 || dormancyBreakRate < 0.0) {
        throw std::invalid_argument("SeedBiology parameters must be positive");
    }
    if (dormancyBreakRate > 1.0) {
        throw std::invalid_argument("Dormancy break rate must be smaller than 1");
    }
    if (germinationSuccess < mortalityDormant) {
        throw std::invalid_argument("Mortality of germinating seeds cannot be smaller than mortality of dormant seeds");
    }
    if (germinationSuccess > 1.0) {
        throw std::invalid_argument("Mortality of germinating seeds must be smaller than 1");
    }

}   