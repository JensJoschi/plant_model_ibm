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

#include "g_GSP_BASE.h"
#include "generalFunctions.h"

/** @cond */
#include <nlohmann/json.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include "easylogging++.h"
/** @endcond */

GSP_BASE::GSP_BASE(): simulDuration(0){
}

GSP_BASE::GSP_BASE(const std::string& configFile){
  nlohmann::json j = generalFunctions::readJsonFile(configFile);
  LOG(INFO) << "Reading global simulation parameters from json file";
  LOG(DEBUG) << SUBSECTIONBREAK << "Reading global simulation parameters from json file.";

  try{simulDuration = j.at("SimulDuration");}
  catch(...) {LOG(FATAL) << "json::SimulDuration was not found.";}

  try {
    m_saveYears = j["SaveYears"].get<std::vector<int>>();
    std::sort(m_saveYears.begin(), m_saveYears.end());
  } catch(...) {
    LOG(WARNING) << "Problem in SaveYears. Saving only last year of simulation run";
    m_saveYears.emplace_back(simulDuration);
  }

  checkContent();
}

void GSP_BASE::defaultBuild(){ //not in use but could be used as fallback option if something goes wrong in file reading.
  LOG(INFO) << "using default global simulation parameters.";
  simulDuration = 2;
}


void GSP_BASE::checkContent() const{
  if (simulDuration <= 0)   LOG(FATAL) << "Error in GSP_BASE: Parameter SIMULATION_DURATION must be superior to 0!";
  if (simulDuration > 1000) LOG(WARNING) << "GSP_BASE: simulation duration was set to " << simulDuration << ". This may take a while.";
  if (m_saveYears.back() > simulDuration)  LOG(FATAL) << "Error in GSP: at least one save time was larger than simulation runtime.";
  LOG(DEBUG) << "***done.";
}
