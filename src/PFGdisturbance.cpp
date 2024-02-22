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
 // RFate team: original idea (RFATE)
 // Jens Joschinski: reimplementation (EPM)
 // --------------------------------------------------------------------------


 #include "PFGdisturbance.h"
/** @cond */
#include "easylogging++.h"
#include "nlohmann/json.hpp"
/** @endcond */

//json file:
// {
//         "Name": "plant_x",
//         "immature":{
//             "fox": {
//                  "SeedKill": 0.1,
//                "PropResprout": 0.0,
//                 "PropKilled": 0.0
//             },
//             "bee":{ 
//                 "SeedKill": 0.0,
//                 "PropResprout": 0.0,
//                "PropKilled": 0.0}
//         },
//         "mature"{...}
//  }

 PFGDisturbances::PFGDisturbances(const nlohmann::json& pfg){
    try {name = pfg.at("Name");} 
        catch(nlohmann::json::out_of_range){LOG(FATAL) << "Error in PFG: Name not found";}

    try {
        for (auto& [lifeStage, dist] : pfg.items()){
            if (lifeStage == "Name") continue;
            LifeStage ls;
            if (lifeStage == "Mature") ls = LifeStage::Mature;
            else if (lifeStage == "Immature") ls = LifeStage::Immature;
            else LOG(FATAL) << "Error in PFG: life stage" << lifeStage << " not found (only Immature and Mature allowed)";

            for (auto& [distName, distParams] : dist.items()){
                const Dist temp{distParams.at("SeedKill"), distParams.at("PropResprout"), distParams.at("PropKilled")};
                distResponse[ls].insert(std::make_pair(distName, temp));
            }
        }
    } catch(nlohmann::json::out_of_range){LOG(FATAL) << "Error in PFG: immature not found";}
 }