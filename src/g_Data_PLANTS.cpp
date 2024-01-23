/*------------------------------------------------------------------------------
Copyright (C) 2023 - present Jens Joschinski

This file is part of the ECOLOPES PLANT MODEL.

ECOLOPES PLANT MODEL is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your option) any later version.

ECOLOPES PLANT MODEL is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with ECOLOPES PLANT MODEL. 
If not, see <https://www.gnu.org/licenses/>. */

// --------------------------------------------------------------------------
 /* ECOLOPES PLANT MODEL is derived, modified and extended from FATE, https://github.com/leca-dev/RFate.git/ 
 * Copyright (C) 2021 Isabelle Boulangeat, Damien Georges, Maya Guéguen, Wilfried Thuiller 
 * For contributions to this particular file, see section "Authors and contributors".*/
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// Authors and contributors to this file:
// Jens Joschinski: Code structure and content
// Victoria Culshaw: split class into submodel classes
// JJ Inheritance and restructuring
// --------------------------------------------------------------------------




#include "g_Data_BASE.h"
#include "g_Data_PLANTS.h"
#include "g_GSP_PLANTS.h"
#include "p_PFGDefs.h"

#include "Landscape.h"
#include "a_generalFunctions_vmvc.h"


/** @cond */
#include "easylogging++.h"
#include <string>
#include <nlohmann/json.hpp>
#include <filesystem>
/** @endcond */




Data_PLANTS::Data_PLANTS(const std::string& paramSimulFile, const GSP_PLANTS& gsp): 
  Data_BASE(paramSimulFile, gsp){ 
  LOG(INFO) << "Adding plant-specific data";
    
  assert (gsp.doesPlants);
  assert (paramSimulFile != "");

  //base ctor has already filled in m_inputDir etc. Now we read the file again to fill the remaining members
  nlohmann::json j = generalFunctions::readJsonFile(paramSimulFile);

  LOG(INFO) << "--Functional Group Definitions";
  //----------------------------------------------------------------------------------------

  PFGDefinitions = PFGDefs();
  std::string pfg_string;
  std::string disturbance_string  ="";
  try{
    pfg_string = j.at("PFGDefsFile");
    try{disturbance_string = j.at("DisturbanceDefsFile"); 
        LOG (DEBUG) << "DisturbanceDefsFile found";
        PFGDefinitions = PFGDefs(inputDir + pfg_string, inputDir + disturbance_string);
        LOG(DEBUG) << "PFGDefs read (with disturbance).";
        } catch (nlohmann::json::out_of_range) {
          LOG(WARNING) << "DisturbanceDefsFile not found";
          PFGDefinitions = PFGDefs(inputDir + pfg_string);
          LOG(DEBUG) << "PFGDefs read (without disturbance).";
        }
  } catch (nlohmann::json::out_of_range) { LOG(FATAL) << "PFGDefsFile not found"; }
    catch(...){LOG(FATAL) <<"uncaught exception in PFGDefs";}

  
  LOG(INFO) << "--Input Data";
  //----------------------------------------------------------------------------------------

if (gsp.doesShadingPercentages){
  try{shading = readFile<double>(j, "ShadingFile", inputDir);}
    catch(nlohmann::json::out_of_range) {
      LOG(WARNING) << "ShadingFile not found"; 
      shading = Landscape<double>();
    }
}
if(gsp.doesSoilDepth){
  try{soilDepth = readFile<int>(j, "DepthFile", inputDir);}
    catch(nlohmann::json::out_of_range) { 
      LOG(WARNING) <<"DepthFile not found";
      soilDepth = Landscape<int>();
    }
}

  checkContent(gsp);
}


void Data_PLANTS::checkContent(const GSP_PLANTS& gsp) const {
    LOG(INFO) << "--Performing checks for plant model data.";
    //check if definitions are exactly equal with regional model, report if there is a mismatch
    //mostly for debugging, issue warning only. Regional model is in fact sometimes a subset of PFGDefs
    std::vector<std::string> defs = PFGDefinitions.getNames();
    std::sort(defs.begin(), defs.end());
    std::vector<std::string> reg = listPlantFunctionalGroups;
    std::sort(reg.begin(), reg.end()); 
    for (int i = 0; i<defs.size(); i++){
      if (defs[i] ==reg[i]) continue;
      else {
        LOG(WARNING) << "Mismatch between regional model and PFG definitions detected.";
        LOG(WARNING) << "-----first mismatch in position : " << i << " ----";
        LOG(WARNING) << "PFGDefs: " << defs[i];
        LOG(WARNING) << "inALL: " << reg[i];
        break;
      }
    }

    if (listPlantFunctionalGroups.size() < PFGDefinitions.size()) {
      LOG(WARNING) << "Warning: Not all defined plants occur on site. Defined: " << 
     PFGDefinitions.size() << "on site: "<<  listPlantFunctionalGroups.size();}
    if (listPlantFunctionalGroups.size() > PFGDefinitions.size()) {
      LOG(FATAL)  << "not all plants on site have been defined. Defined: "<< 
     PFGDefinitions.size()<< "on site: " << listPlantFunctionalGroups.size();}
    //temporary:
    if (listPlantFunctionalGroups.size() !=PFGDefinitions.size()){
      LOG(FATAL) << "regional model temporarily deactivated. "<<
      "Please make sure that the number of PFGs in main is the same as GSP::no_PFG. Defined: "<< 
      PFGDefinitions.size()<< "on site: "<<  listPlantFunctionalGroups.size();
    }
    
    //todo here: check PFGDefs against global params. currently PFGDefs 
    //could have three strata, but global could indicate 4.

    if (gsp.doesShadingPercentages){
      LOG(DEBUG) << "light";
      if (shading.getTotncell()== 0){
        LOG(FATAL)  << "shading values: " << shading.getTotncell();
      }
      for (const auto& it : shading){
        if (it.second > 1.0 || it.second < 0.0) LOG(FATAL) << "Shading map contains values outside of [0,1]";
      }
    }

    if (gsp.doesSoilDepth){ 
      LOG(DEBUG) << "habsuit";
      if(soilDepth.getTotncell() == 0) LOG (WARNING) << "soil depth map contains" << soilDepth.getTotncell() << "elements. not used.";
      for (const auto& it : soilDepth){
        if (it.second < 0) { LOG(FATAL) << "Habsuit map contains values smaller than 0";}
      }
    }

  LOG(INFO) << "***data checks for plant model done.";
}

bool Data_PLANTS::checkKeys(const GSP_PLANTS& gsp) const{
  if (gsp.doesShadingPercentages && (shading.getTotncell() != keyList.getKeys().size())) return false; 
  if (gsp.doesSoilDepth && (soilDepth.getTotncell() != keyList.getKeys().size())) return false;

  for (const auto& key : keyList.getKeys()){
    if (keyList.count(key) != 1) return false;
    if (gsp.doesShadingPercentages && shading.count(key) != 1) return false;
    if (gsp.doesSoilDepth && soilDepth.count(key) != 1) return false;
  }

  return true;
}