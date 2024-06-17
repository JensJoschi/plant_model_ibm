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
// Jens Joschinski: Code structure and content (ECOLOPES JOINT MODEL)
// Victoria Culshaw: split class into submodel classes (ECOLOPES JOINT MODEL)
// JJ Inheritance and restructuring (ECOLOPES JOINT MODEL/EPM)
// JJ update to new model (IBM)
// --------------------------------------------------------------------------




#include "g_Data_BASE.h"
#include "Data_PLANTS.h"
#include "GSP_PLANTS.h"

#include "Landscape.h"
#include "generalFunctions.h"


/** @cond */
#include "easylogging++.h"
#include <string>
#include <nlohmann/json.hpp>
#include <filesystem>
/** @endcond */




Data_PLANTS::Data_PLANTS(const nlohmann::json& j, const GSP_PLANTS& gsp): 
  Data_BASE(j, gsp){ 
  assert(gsp.simulDuration > 0);

  LOG(INFO) << "Adding plant-specific data";
  assert (!j.empty());

  try{logger = j.at("PlantLogger");}
  catch(nlohmann::json::out_of_range) {logger = "plantlog.conf";}
  catch(...){LOG(FATAL) << "uncaught exception in logger";}
  
  LOG(INFO) << "--Input Data";

  // if (gsp.doesShadingPercentages){
    try{light = readFile<double>(j, "LightFile", inputDir);}
      catch(nlohmann::json::out_of_range) {
        LOG(WARNING) << "LightFile not found"; 
        light = Landscape<double>();
      }
  // }
  if(gsp.doesSoilDepth){
    try{soilDepth = readFile<int>(j, "DepthFile", inputDir);}
      catch(nlohmann::json::out_of_range) { 
        LOG(WARNING) <<"DepthFile not found";
        soilDepth = Landscape<int>();
      }
  }

  if(gsp.doesSoilClass){
    try{soilClass = readFile<std::string>(j, "SoilClassFile", inputDir);}
      catch(nlohmann::json::out_of_range) { 
        LOG(WARNING) <<"SoilClassFile not found";
        soilClass = Landscape<std::string>();
      }
  }

  if (gsp.doesDisturbance){
    try{management = readFile<std::map<std::string, double>>(j, "ManagementFile", inputDir);}
    catch(nlohmann::json::out_of_range) { LOG(FATAL) << "Management not found.";}
    }

 if (gsp.doesRegionalModel){
    try{allowedTypes = readFile<std::vector<std::string>>(j, "RegionalModelFile", inputDir);}
    catch(nlohmann::json::out_of_range) { LOG(FATAL) << "RegionalModelFile not found.";}
}

  checkContent(gsp);
}


void Data_PLANTS::checkContent(const GSP_PLANTS& gsp) const {
    LOG(INFO) << "--Performing checks for plant model data.";

    // if (gsp.doesShadingPercentages){
      LOG(DEBUG) << "light";
      if (light.size()== 0){
        LOG(FATAL)  << "no shading values.";
      }
      for (const auto& it : light){
        if (it.second < 0.0) LOG(FATAL) << "light is negative in at least one cell.";
      }
    // }

    if (gsp.doesSoilDepth){ 
      LOG(DEBUG) << "habsuit";
      if(soilDepth.size() == 0) LOG (WARNING) << "soil depth map contains 0 elements.";
      for (const auto& it : soilDepth){
        if (it.second < 0) { LOG(FATAL) << "soil depth map contains values smaller than 0";}
      }
    }

    if(gsp.doesSoilClass){
      LOG(DEBUG) << "soil class";
      if(soilClass.size() == 0) LOG (WARNING) << "soil class map contains 0 elements.";
      for (const auto& it : soilClass){
        if (it.second == "") { LOG(FATAL) << "Soil class map contains empty strings";}
      }
    }

    if(gsp.doesDisturbance){
      LOG(DEBUG) << "Management";
      if(management.size() == 0) LOG (FATAL) << "map of management wrong";
    }

  LOG(INFO) << "***data checks for plant model done.";
}

bool Data_PLANTS::checkKeys(const GSP_PLANTS& gsp) const{
  if (light.size() != keyList.size()) return false; 
  if (gsp.doesSoilDepth && (soilDepth.size() != keyList.size())) return false;
  if (gsp.doesSoilClass && (soilClass.size() != keyList.size())) return false;
  if (gsp.doesDisturbance && (management.size() != keyList.size())) return false;

  for (const auto& key : keyList){
    if (light.count(key) != 1) return false;
    if (gsp.doesSoilDepth && soilDepth.count(key) != 1) return false;
    if (gsp.doesSoilClass && soilClass.count(key) != 1) return false;
    if (gsp.doesDisturbance && management.count(key) != 1) return false;
  }

  return true;
}