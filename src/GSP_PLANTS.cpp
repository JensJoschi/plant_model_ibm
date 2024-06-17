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
 // RFate team (RFATE)
 // JJ: integrated animal and soil parameters and removed fire, alien and drought modules.
 //     changed default constructor to create a useful set of parameters 
 //     added error checking, json integration (ECOLOPES JOINT MODEL)
 // VC: split global params class into subclasses (ECOLOPES JOINT MODEL)
 // JJ: inheritance and rewrite (ECOLOPES JOINT MODEL/EPM)
 // JJ (IBM): adjust to new parameters
 // --------------------------------------------------------------------------

 #include "GSP_PLANTS.h"
 #include "g_GSP_BASE.h"
 /** @cond */
 #include <fstream>
 #include <string>
 #include <vector>
 #include "nlohmann/json.hpp"
 #include "easylogging++.h"
/** @endcond */



/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* Constructors                                                                                    */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

GSP_PLANTS::GSP_PLANTS(const nlohmann::json& configFile): GSP_BASE(configFile){
  LOG(INFO) << "Adding plant-specific global simulation parameters";
  addSpecificParams(configFile);
  checkContent();
}


void GSP_PLANTS::addSpecificParams(const nlohmann::json& j){
  LOG(INFO) << SUBSECTIONBREAK << "Adding plant-specific params";

  LOG(INFO) << "--Parameters";
  try{noStrata = j.at("NoStrata");}
  catch(nlohmann::json::out_of_range) { LOG(FATAL) << "json::NoStrata not found.";}
  catch(nlohmann::json::type_error)   { LOG(FATAL) << "json::NoStrata not integer.";}
  catch(nlohmann::json::exception& e) { LOG(FATAL) << "json::NoStrata error: " << e.what();}
  try{voxelHeight = j.at("VoxelHeight");}
  catch(nlohmann::json::out_of_range) { LOG(FATAL) << "json::VoxelHeight not found.";}
  catch(nlohmann::json::type_error)   { LOG(FATAL) << "json::VoxelHeight  not float.";}
  catch(nlohmann::json::exception& e) { LOG(FATAL) << "json::VoxelHeight error: " << e.what();}
  try{voxelArea = j.at("VoxelArea");}
  catch(nlohmann::json::out_of_range) { LOG(FATAL) << "json::VoxelArea not found.";}
  catch(nlohmann::json::type_error)   { LOG(FATAL) << "json::VoxelArea not float.";}
  catch(nlohmann::json::exception& e) { LOG(FATAL) << "json::VoxelArea error: " << e.what();}

  try{startingPopSize = j.at("StartingPopSize");}
  catch(nlohmann::json::out_of_range) { LOG(FATAL) << "json::StartingPopSize not found.";}
  catch(nlohmann::json::type_error)   { LOG(FATAL) << "json::StartingPopSize not integer.";}
  catch(nlohmann::json::exception& e) { LOG(FATAL) << "json::StartingPopSize error: " << e.what();}

  try{initialSeeds = j.at("InitialSeeds");}
  catch(nlohmann::json::out_of_range) { LOG(WARNING) << "json::InitialSeeds not found. Set to 0"; initialSeeds = 0;}
  catch(nlohmann::json::type_error)   { LOG(FATAL) << "json::InitialSeeds not integer.";}
  catch(nlohmann::json::exception& e) { LOG(FATAL) << "json::InitialSeeds error: " << e.what();}

  try{lightDistributionFactor = j.at("LightDistributionFactor");}
  catch(nlohmann::json::out_of_range) { LOG(FATAL) << "json::LightDistributionFactor not found.";}
  catch(nlohmann::json::type_error)   { LOG(FATAL) << "json::LightDistributionFactor not integer.";}
  catch(nlohmann::json::exception& e) { LOG(FATAL) << "json::LightDistributionFactor error: " << e.what();}

  try{diffusion = j.at("Diffusion");}
  catch(nlohmann::json::out_of_range) { LOG(FATAL) << "json::Diffusion not found.";}
  catch(nlohmann::json::type_error)   { LOG(FATAL) << "json::Diffusion not float.";}
  catch(nlohmann::json::exception& e) { LOG(FATAL) << "json::Diffusion error: " << e.what();}

  try{doesSoilClass = j.at("DoesSoilClass");}
  catch(nlohmann::json::out_of_range) { LOG(FATAL) << "json::DoesSoilClass not found.";}
  catch(nlohmann::json::type_error)   { LOG(FATAL) << "json::DoesSoilClass not boolean.";}
  catch(nlohmann::json::exception& e) { LOG(FATAL) << "json::DoesSoilClass error: " << e.what();}
  try{doesSoilDepth = j.at("DoesSoilDepth");}
  catch(nlohmann::json::out_of_range) { LOG(FATAL) << "json::DoesSoilDepth not found.";}
  catch(nlohmann::json::type_error)   { LOG(FATAL) << "json::DoesSoilDepth not boolean.";}
  catch(nlohmann::json::exception& e) { LOG(FATAL) << "json::DoesSoilDepth error: " << e.what();}
  try{soilCapacity = j.at("SoilCapacity");}
  catch(nlohmann::json::out_of_range) { LOG(FATAL) << "json::SoilCapacity not found.";}
  catch(nlohmann::json::type_error)   { LOG(FATAL) << "json::SoilCapacity not integer.";}
  catch(nlohmann::json::exception& e) { LOG(FATAL) << "json::SoilCapacity error: " << e.what();}
  try{doesDispersal = j.at("DoesDispersal");}
  catch(nlohmann::json::out_of_range) { LOG(FATAL) << "json::DoesDispersal not found.";}
  catch(nlohmann::json::type_error)   { LOG(FATAL) << "json::DoesDispersal not boolean.";}
  catch(nlohmann::json::exception& e) { LOG(FATAL) << "json::DoesDispersal error: " << e.what();}
  try{doesRegionalModel = j.at("DoesRegionalModel");}
  catch(nlohmann::json::out_of_range) { LOG(FATAL) << "json::DoesRegionalModel not found.";}
  catch(nlohmann::json::type_error)   { LOG(FATAL) << "json::DoesRegionalModel not boolean.";}
  catch(nlohmann::json::exception& e) { LOG(FATAL) << "json::DoesRegionalModel error: " << e.what();}


  try{seedRain = j.at("SeedRain");}
  catch(nlohmann::json::out_of_range) { LOG(WARNING) << "json::SeedRain not found. Set to 0"; seedRain = 0;}
  catch(nlohmann::json::type_error)   { LOG(FATAL) << "json::SeedRain not integer.";}
  catch(nlohmann::json::exception& e) { LOG(FATAL) << "json::SeedRain error: " << e.what();}
}


void GSP_PLANTS::checkContent() const{
  // LOG(INFO) << "--Performing checks";

//   if (noStrata <= 0)                LOG(FATAL) << "Error in GSP:  Parameter NO_STRATA must be superior to 0!";
//   if (strataHeight.size() != noStrata) LOG(FATAL) << "Error in GSP:  Parameter STRATA_HEIGHT must have the same size as NO_STRATA!";
//   for (auto i : strataHeight){
//     if (i < 0) LOG(FATAL) << "Error in GSP:  Parameter STRATA_HEIGHT must be superior to 0!";
//   }

//   if (doesPlantDispersal & (seedInput <= 0))      LOG(FATAL) << "Error in GSP:  Parameter SEEDING_INPUT must be superior to 0!";

//   LOG(INFO) << "--all checks passed";
  }
