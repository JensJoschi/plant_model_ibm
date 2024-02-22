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

#include "LifeHistory.h"

/** @cond */
#include "nlohmann/json.hpp"
#include "easylogging++.h"
/** @endcond */



LifeHistory::LifeHistory(const nlohmann::json& traits) {
    try {
        if (traits.at("MaturationTime").is_number_integer()) {
            M = traits.at("MaturationTime").get<int>();
        } else {
            throw std::runtime_error("type of MatTime must be integer, but is " + 
            std::string(traits.at("MaturationTime").type_name()));
        }
        if (traits.at("LifeSpan").is_number_integer()) {
            L = traits.at("LifeSpan").get<int>();
        } else {
            throw std::runtime_error("type of LifeSpan must be integer, but is " + 
            std::string(traits.at("LifeSpan").type_name()));
        }
        if (traits.at("MaxHeight").is_number_integer()) {
            HMax = traits.at("MaxHeight").get<int>();
        } else {
            throw std::runtime_error("type of MaxHeight must be integer, but is " + 
            std::string(traits.at("MaxHeight").type_name()));
        }
        ShadeFactor = traits.at("ShadeFactor").get<double>();
        check();
    } catch (nlohmann::json::exception& e) {
        LOG(DEBUG) << "fields in life history file are: " << traits.dump(4);
        LOG(ERROR) << "Exception when reading from json LifeHistory: " << e.what() << '\n';
        throw;
    } catch (std::runtime_error& e) {
        LOG(ERROR) << "Runtime error when creating LifeHistory: " << e.what() << '\n';
        throw;
    } catch (std::invalid_argument& e) {
        LOG(ERROR) << "Invalid argument when initializing LifeHistory: " << e.what() << '\n';
        throw;
    }
}

void LifeHistory::check() {
    if (M < 0 || L < 0 || HMax < 0 || ShadeFactor < 0.0) {
        throw std::invalid_argument("LifeHistory parameters must be positive");
    }
    if (M > L) {
        throw std::invalid_argument("Maturation time must be smaller than life span");
    }
    if (ShadeFactor > 1.0) {
        throw std::invalid_argument("Shade factor must be smaller than 1.0");
    }
}   