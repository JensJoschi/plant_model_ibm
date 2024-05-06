
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


/* This particular file is derived and altered from the ECOLOPES JOINT MODEL, 
 Copyright (C) 2022 - present Studio Animal-Aided Design and TU Munich

 ECOLOPES JOINT MODEL is based on:
 - the ECOLOPES PLANT MODEL, 2022 - present  Studio Animal-Aided Design
 - the ECOLOPES ANIMAL MODEL, Copyright (C) 2022 - present TU Munich
 - and the ECOLOPES SOIL MODEL,2022 - present  Studio Animal-Aided Design


 // --------------------------------------------------------------------------
 // Authors and contributors to this file:
 // JJ: Code and most content of this base class (partially based on FATE but heavily altered)
 // VC: splitting into general and submodel classes
 // JJ: inheritance and rewrite
 // JJ(IBM): use json objects, removal of outdated capabilities. It is hoped that JOINT model
 // will eventually merge and adopt those changes, and incorporate the new IBM model
 // --------------------------------------------------------------------------
 */

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

GSP_BASE::GSP_BASE(const nlohmann::json& configFile){

  try{simulDuration = configFile.at("SimulDuration");}
  catch (const nlohmann::json::out_of_range& e) {LOG(FATAL) << "Error: Parameter SimulDuration not found in config file.";}
  catch (const nlohmann::json::type_error& e) {LOG(FATAL) << "Error: Parameter SimulDuration is of wrong type.";}
  catch (const nlohmann::json::exception& e) {LOG(FATAL) << "Error: Problem with SimulDuration: "<< e.what();}

  try {
    m_saveYears = configFile["SaveYears"].get<std::vector<int>>();
    std::sort(m_saveYears.begin(), m_saveYears.end());
  } 
  catch (const nlohmann::json::out_of_range& e) {
    LOG(WARNING) << "Parameter SaveYears not found in config file. Saving only last year of simulation run";
    m_saveYears.emplace_back(simulDuration);}
  catch (const nlohmann::json::type_error& e) {LOG(FATAL) << "Error: Parameter SaveYears is of wrong type.";}
  catch (const nlohmann::json::exception& e) {LOG(FATAL) << "Error: Problem with SaveYears: "<< e.what();}
  checkContent();
}



void GSP_BASE::checkContent() const{
  if (simulDuration <= 0)   LOG(FATAL) << "Error in GSP_BASE: Parameter SIMULATION_DURATION must be superior to 0!";
  if (simulDuration > 1000) LOG(WARNING) << "GSP_BASE: simulation duration was set to " << simulDuration << ". This may take a while.";
  if (m_saveYears.back() > simulDuration)  LOG(FATAL) << "Error in GSP: at least one save time was larger than simulation runtime.";
  LOG(DEBUG) << "***done.";
}
