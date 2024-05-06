/* Copyright (C) 2022 - present Studio Animal-Aided Design and TU Munich
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
 - the ECOLOPES PLANT MODEL, 2022 - present  Studio Animal-Aided Design
 - the ECOLOPES ANIMAL MODEL, Copyright (C) 2022 - present TU Munich
 - and the ECOLOPES SOIL MODEL,2022 - present  Studio Animal-Aided Design

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

Data_BASE::Data_BASE(const nlohmann::json& paramSimulFile, const GSP_BASE& gsp){
  LOG(INFO) << SUBSECTIONBREAK << "Reading basic data from json file.";

  //----------------------------------------------------------------------------
  LOG(DEBUG) << "--Data sources";

  try{inputDir = paramSimulFile.at("InputDir");}
    catch(nlohmann::json::out_of_range) { LOG(WARNING) << "InputDir not found. Set to 0-INPUT/"; inputDir = "0-INPUT/";}
    catch(nlohmann::json::type_error) { LOG(FATAL) << "InputDir variable has wrong type.";}
    catch(nlohmann::json::exception& e) { LOG(FATAL) << "InputDir error: " << e.what();}

  try{savingDir = paramSimulFile.at("SaveDir");}
    catch(nlohmann::json::out_of_range) { LOG(WARNING) << "Saving dir not found. Using 0-RESULTS/"; savingDir = "0-RESULTS/";}
    catch(nlohmann::json::type_error) { LOG(FATAL) << "SavingDir variable has wrong type.";}
    catch(nlohmann::json::exception& e) { LOG(FATAL) << "SavingDir error: " << e.what();}
  
  //----------------------------------------------------------------------------------------
  LOG(DEBUG) << "--Input Data";
  try{keyList = readFile<double>(paramSimulFile, "MaskFile" , inputDir);}
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