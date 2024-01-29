/* Copyright (C) 2023 - present ???
 This file is part of the ECOLOPES JOINT MODEL.

 ECOLOPES JOINT MODEL is free software: you can redistribute it and/or modify 
 it under the terms of the GNU General Public License as published by the 
 Free Software Foundation, either version 3 of the License, or (at your option) any later version.

 ECOLOPES JOINT MODEL is distributed in the hope that it will be useful, 
 but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with ECOLOPES PLANT MODEL. 
 If not, see <https://www.gnu.org/licenses/>.

 ECOLOPES JOINT MODEL is based on:
 - the ECOLOPES PLANT MODEL, Copyright (C) 2023 ???
 - the ECOLOPES ANIMAL MODEL, Copyright (C) 2023 ???
 - and the ECOLOPES SOIL MODEL, Copyright (C) 2023 ???

*/


 // --------------------------------------------------------------------------
 // Authors and contributors to this file:
 // JJ: Code and most content of this base class (partially based on FATE but heavily altered)
 // VC: splitting into general and submodel classes
 // JJ: inheritance and rewrite
 // --------------------------------------------------------------------------
 

#include "g_Data_BASE.h"
#include "g_GSP_BASE.h"

/** @cond */
#include <string>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include <fstream>
#include "easylogging++.h"
/** @endcond */


#include "generalFunctions.h"
#include "Landscape.h"

Data_BASE::Data_BASE(const std::string& paramSimulFile, const GSP_BASE& gsp){
  LOG(DEBUG) << SUBSECTIONBREAK << "Reading basic data from json file.";
  LOG(INFO) << "Reading basic data from json file.";

  //----------------------------------------------------------------------------
  LOG(DEBUG) << "--Data sources";
  nlohmann::json j = generalFunctions::readJsonFile(paramSimulFile);

  try{inputDir = j.at("InputDir");}
    catch(nlohmann::json::out_of_range) { LOG(WARNING) << "InputDir not found. Set to 0-INPUT/"; inputDir = "0-INPUT/";}
  try{savingDir = j.at("SaveDir");}
    catch(nlohmann::json::out_of_range) {  LOG(WARNING) << "Saving dir not found. Using 0-RESULTS/"; savingDir = "0-RESULTS/";}
  
  //----------------------------------------------------------------------------
  LOG(DEBUG) << "--regional model information";
//temporary, in reality it should check if regional model has been provided
//ideally it should also compare against PFG/AFG/soil definitions, but it does not know them 
  try{listPlantFunctionalGroups = j.at("listPlantFunctionalGroups");}
    catch(nlohmann::json::out_of_range) {  LOG(FATAL) << "listPlantFunctionalGroups not found";}
  
  try{listAnimalFunctionalGroups = j.at("listAnimalFunctionalGroups");}
    catch(nlohmann::json::out_of_range) {  LOG(FATAL) << "listAnimalFunctionalGroups not found";}
  
  std::sort(listPlantFunctionalGroups.begin(), listPlantFunctionalGroups.end());
  std::sort(listAnimalFunctionalGroups.begin(), listAnimalFunctionalGroups.end());
  //----------------------------------------------------------------------------------------
  LOG(DEBUG) << "--Input Data";
  try{keyList = readFile<double>(j, "MaskFile" , inputDir);}
  catch(nlohmann::json::out_of_range) {   
     LOG(FATAL) <<"MaskFile not found"; 
  }
  checkContent(gsp);
}


void Data_BASE::checkContent(const GSP_BASE& gsp) const{

  LOG(DEBUG) << "--Performing checks";
  std::filesystem::create_directory(inputDir);  //will not create a new one if already present
  std::filesystem::create_directory(savingDir);

  LOG(DEBUG) << "--All checks done";
}


bool Data_BASE::checkKeys(const GSP_BASE& config) const{
    return true;
}