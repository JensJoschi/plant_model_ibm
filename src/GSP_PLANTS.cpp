/*------------------------------------------------------------------------------
Copyright (C)  2022 - present  Studio Animal-Aided Design

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
 // RFate team
 // JJ: integrated animal and soil parameters and removed fire, alien and drought modules.
 //     changed default constructor to create a useful set of parameters
 //     added error checking, json integration
 // VC: split global params class into subclasses
 // JJ: inheritance and rewrite
 // --------------------------------------------------------------------------

 #include "GSP_PLANTS.h"
 #include "g_GSP_BASE.h"
 #include "generalFunctions.h"
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


GSP_PLANTS::GSP_PLANTS():GSP_BASE() { //calls base default ctor and then adds stuff
  // defaultBuild(); //leaving eveything false/zero for now
}

void GSP_PLANTS::defaultBuild(){
  doesNeighbourinfluence = false;
  doesShadingPercentages = true;
  doesHabSuitability = true; 
  doesSoilClass = true;
  doesSoilDepth = true; 
  doesPlantDispersal = true;  
  doesDisturbance =true; 

  noStrata = 5;
  strataHeight = std::vector <int> {25, 25, 25, 25, 25};
  seedInput = 100;
  potentialFecundity = 1;
  maxAbundLow = 1000;
  maxAbundMedium = 2000; 
  maxAbundHigh = 3000;
  lightThreshLow = 6000;
  lightThreshMedium = 4000; 
  lightAngle = 90;
}

GSP_PLANTS::GSP_PLANTS(const std::string& configFile): GSP_BASE(configFile){
  LOG(INFO) << "Adding plant-specific global simulation parameters";
  addSpecificParams(configFile);
  checkContent();
}


void GSP_PLANTS::addSpecificParams(const std::string& configFile){
  nlohmann::json j = generalFunctions::readJsonFile(configFile);
  LOG(INFO) << SUBSECTIONBREAK << "Adding plant-specific params";

  LOG(INFO) << "--Parameters";
  try{noStrata = j.at("NoStrata");}
  catch(nlohmann::json::out_of_range) { LOG(WARNING) << "json::NoStrata not found. Set to 1";       noStrata = 1;}
  catch(nlohmann::json::type_error)   { LOG(FATAL) << "json::NoStrata given but not integer.";}
  try{strataHeight = j.at("StrataHeight").get<std::vector<int>>();}
  catch(nlohmann::json::out_of_range) { LOG(WARNING) << "json::StrataHeight not found. Set to 1";       strataHeight = std::vector <int> (noStrata, 1);}
  catch(nlohmann::json::type_error)   { LOG(FATAL) << "json::StrataHeight given but not vector of integer.";}

  try{potentialFecundity = j.at("PotentialFecundity");}
  catch(nlohmann::json::out_of_range) { LOG(WARNING) << "json::PotentialFecundity not found. Set to 1";       potentialFecundity = 1;}
  catch(nlohmann::json::type_error)   { LOG(FATAL) << "json::PotentialFecundity given but not integer.";}

  try{maxAbundLow = j.at("MaxAbundLow");}
  catch(nlohmann::json::out_of_range) { LOG(WARNING) << "json::MaxAbundLow not found. Set to 1000";       maxAbundLow = 1000;}
  catch(nlohmann::json::type_error)   { LOG(FATAL) << "json::MaxAbundLow given but not integer.";}

  try{maxAbundMedium = j.at("MaxAbundMedium");}
  catch(nlohmann::json::out_of_range) { LOG(WARNING) << "json::MaxAbundMedium not found. Set to 2000";       maxAbundMedium = 2000;}
  catch(nlohmann::json::type_error)   { LOG(FATAL) << "json::MaxAbundMedium given but not integer.";}

  try{maxAbundHigh = j.at("MaxAbundHigh");}
  catch(nlohmann::json::out_of_range) { LOG(WARNING) << "json::MaxAbundHigh not found. Set to 3000";       maxAbundHigh = 3000;}
  catch(nlohmann::json::type_error)   { LOG(FATAL) << "json::MaxAbundHigh given but not integer.";}

//------------------------------------dispersal------------------------------------
  try{doesPlantDispersal = j.at("DoPlantDispersal");}
  catch(nlohmann::json::out_of_range) { LOG(WARNING) <<"json::DoPlantDispersal not found. Set to false";   doesPlantDispersal = false;}
  catch(nlohmann::json::type_error)   { LOG(FATAL) << "json::DoPlantDispersal given but not boolean.";}
  if (doesPlantDispersal){
    try{seedInput = j.at("SeedingInput");}
    catch(nlohmann::json::out_of_range) { LOG(WARNING) << "json::SeedingInput not found. Set to 1";       seedInput = 1;}
    catch(nlohmann::json::type_error)   { LOG(FATAL) << "json::SeedingInput given but not integer.";}
  }

//------------------------------------shading------------------------------------
  // try{m_DoShading = j.at("DoLightInteraction");}
  // catch(nlohmann::json::out_of_range) { logg.warning("json::DoLightInteraction not found. Set to true"); m_DoShading = true;}
  // catch(nlohmann::json::type_error)   { logg.error("json::DoLightInteraction given but not boolean.");}
//if (m_DoShading){
    try{doesShadingPercentages =j.at("DoShadingPercentages");}
    catch(nlohmann::json::out_of_range) { LOG(WARNING) << "json::DoShadingPercentages not found. Set to true"; doesShadingPercentages = true;}
    catch(nlohmann::json::type_error)   { LOG(FATAL) << "json::DoShadingPercentages given but not boolean.";}

    try{lightThreshLow = j.at("LightThresLow");}
    catch(nlohmann::json::out_of_range) { LOG(WARNING) << "json::LightThresLow not found. Set to 6000";       lightThreshLow = 6000;}
    catch(nlohmann::json::type_error)   { LOG(FATAL) << "json::LightThresLow given but not integer.";}

    try{lightThreshMedium = j.at("LightThreshMedium");}
    catch(nlohmann::json::out_of_range) { LOG(WARNING) << "json::LightThreshMedium not found. Set to 4000";       lightThreshMedium = 4000;}
    catch(nlohmann::json::type_error)   { LOG(FATAL) << "json::LightThreshMedium given but not integer.";}

    try{doesNeighbourinfluence = j.at("DoNeighbours");}
    catch(nlohmann::json::out_of_range) { LOG(WARNING) << "json::DoNeighbours not found. Set to false";       doesNeighbourinfluence = false;}
    catch(nlohmann::json::type_error)   { LOG(FATAL) << "json::DoNeighbours given but not boolean.";}

    if (!doesNeighbourinfluence) lightAngle = 90; 
    else{
      try{lightAngle = j.at("LightAngle");}
      catch(nlohmann::json::out_of_range) { LOG(WARNING) << "json::LightAngle not found. Set to 90";       lightAngle = 90;}
      catch(nlohmann::json::type_error)   { LOG(FATAL) << "json::LightAngle given but not integer.";}
    }
//}
//--------------------------habitat suitability------------------------------------ 
  try{doesHabSuitability = j.at("DoHabSuitability");}
  catch(nlohmann::json::out_of_range) { LOG(WARNING) <<"json::DoHabSuitability not found. Set to false";   doesHabSuitability = false;}
  catch(nlohmann::json::type_error)   { LOG(FATAL) << "json::DoHabSuitability given but not boolean.";}

  if(doesHabSuitability){
    try{doesSoilClass = j.at("DoSoilClass");}
    catch(nlohmann::json::out_of_range) { LOG(WARNING) << "json::DoSoilClass not found. Set to true";   doesSoilClass = true;}
    catch(nlohmann::json::type_error)   { LOG(FATAL) << "json::DoSoilClass given but not boolean.";}

    try{doesSoilDepth = j.at("DoSoilDepth");}
    catch(nlohmann::json::out_of_range) { LOG(WARNING) << "json::DoSoilDepth not found. Set to true";   doesSoilDepth = true;}
    catch(nlohmann::json::type_error)   { LOG(FATAL) << "json::DoSoilDepth given but not boolean.";}
  }

//------------------------------------disturbance------------------------------------

try{doesDisturbance = j.at("DoDisturbance");}
catch(nlohmann::json::out_of_range) { LOG(WARNING) << "json::DoDisturbance not found. Set to true";   doesDisturbance = true;}
catch(nlohmann::json::type_error)   { LOG(FATAL) << "json::DoDisturbance given but not boolean.";}  
}

void GSP_PLANTS::checkContent() const{
  LOG(INFO) << "--Performing checks";

  if (noStrata <= 0)                LOG(FATAL) << "Error in GSP:  Parameter NO_STRATA must be superior to 0!";
  if (strataHeight.size() != noStrata) LOG(FATAL) << "Error in GSP:  Parameter STRATA_HEIGHT must have the same size as NO_STRATA!";
  for (auto i : strataHeight){
    if (i < 0) LOG(FATAL) << "Error in GSP:  Parameter STRATA_HEIGHT must be superior to 0!";
  }
  if (potentialFecundity <= 0)      LOG(FATAL) << "Error in GSP:  Parameter POTENTIAL_FECUNDITY must be superior to 0!";
  if (maxAbundLow > maxAbundMedium) LOG(FATAL) << "Error in GSP:  Parameter MAX_ABUND_LOW must be inferior or equal to MAX_ABUND_MEDIUM!";
  if (maxAbundMedium > maxAbundHigh)LOG(FATAL) << "Error in GSP:  Parameter MAX_ABUND_MEDIUM must be inferior or equal to MAX_ABUND_HIGH!";
  if (maxAbundHigh <= 0)            LOG(FATAL) << "Error in GSP:  Parameter MAX_ABUND_HIGH must be superior to 0!";

  if (doesPlantDispersal & (seedInput <= 0))      LOG(FATAL) << "Error in GSP:  Parameter SEEDING_INPUT must be superior to 0!";
  //if (doesShading){
    if (lightThreshLow <= 0)      LOG(FATAL) << "Error in GSP:  Parameter LIGHT_THRESH_LOW must be superior to 0!";
    if (lightThreshMedium <= 0)   LOG(FATAL) << "Error in GSP:  Parameter LIGHT_THRESH_MEDIUM must be superior to 0!";
    if (lightThreshLow < lightThreshMedium) LOG(FATAL) << "Error in GSP:  Parameter LIGHT_THRESH_LOW must be superior or equal to LIGHT_THRESH_MEDIUM!";
//} else if (doesShadingPercentages) logg.error ("Error in GSP:  Parameter DO_SHADING_PERCENTAGES must be false if DO_SHADING is false!");
  if (!doesNeighbourinfluence && (lightAngle != 90)) LOG(WARNING) << "Error in GSP:  Parameter LIGHT_ANGLE given but DO_NEIGHBOURS is false!";
  if (doesNeighbourinfluence && (lightAngle == 90)) LOG(WARNING) << "Error in GSP: DO_NEIGHBOURS is true! but light angle is 90° (DoNeighbours has no effect)";
  if (lightAngle>90 || lightAngle<0) LOG(FATAL) << "Error in GSP:  Parameter LIGHT_ANGLE must be between 0 and 90!";
  
  if(!doesHabSuitability && doesSoilClass) LOG(FATAL) << "Error in GSP: Parameter DO_SOIL_CLASS must be false if DO_HAB_SUITABILITY is false!";
  if(!doesHabSuitability && doesSoilDepth) LOG(FATAL) << "Error in GSP: Parameter DO_SOIL_DEPTH must be false if DO_HAB_SUITABILITY is false!";
  if (doesHabSuitability && !doesSoilClass & !doesSoilDepth) LOG(FATAL) << "Error in GSP: Parameter DO_SOIL_CLASS and/or DO_SOIL_DEPTH must be true if DO_HAB_SUITABILITY is true!";

  LOG(INFO) << "--all checks passed";
  }
