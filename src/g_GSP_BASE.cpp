//This file is not meant for sharing. If you have received the file in error, please email us immediately at info@ecolopes.org

// --------------------------------------------------------------------------
// TEMPORARY. PLANT MODEL SHOULD NOT INCLUDE JOINT MODEL CODE IN FOLDER SRC. 
// --------------------------------------------------------------------------


 // --------------------------------------------------------------------------
 // Authors and contributors to this file:
 // JJ: Code and most content of this base class (partially based on FATE but heavily altered)
 // VC: splitting into general and submodel classes
 // JJ: inheritance and rewrite
 // --------------------------------------------------------------------------

#include "g_GSP_BASE.h"
#include "a_generalFunctions_vmvc.h"

/** @cond */
#include <nlohmann/json.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include "easylogging++.h"
/** @endcond */

GSP_BASE::GSP_BASE(): simulDuration(0), doesPlants(false), doesAnimals(false), doesSoil(false), doesManagement(false){
}

GSP_BASE::GSP_BASE(const std::string& configFile){
  nlohmann::json j = generalFunctions::readJsonFile(configFile);
  LOG(INFO) << "Reading global simulation parameters from json file";
  LOG(DEBUG) << SUBSECTIONBREAK << "Reading global simulation parameters from json file.";
  
  try{simulDuration = j.at("SimulDuration");}
  catch(...) {LOG(FATAL) << "json::SimulDuration was not found.";}

  
  try {
      simulDuration = j.at("SimulDuration");
      doesSoil = j.at("DoSoilModel");
      doesPlants = j.at("DoPlantModel");
      doesAnimals = j.at("DoAnimalModel");
      doesManagement = j.at("DoManagement");
    } catch (const std::exception& e) {
      LOG(FATAL) << "Error reading global simulation parameters from json file: " << e.what();
    }
  try {m_saveYears = j["SaveYears"].get<std::vector<int>>();
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

  doesPlants = true;
  doesAnimals = true; 
  doesSoil =true;
  doesManagement = true;
}


void GSP_BASE::checkContent() const{
  if (simulDuration <= 0)   LOG(FATAL) << "Error in GSP_BASE: Parameter SIMULATION_DURATION must be superior to 0!";
  if (simulDuration > 1000) LOG(WARNING) << "GSP_BASE: simulation duration was set to " << simulDuration << ". This may take a while.";

  if (doesSoil && !doesPlants)         LOG(FATAL) << "Error in GSP: soil model should only be enabled in conjuction with plant model";
  if (!doesAnimals && !doesPlants)     LOG(FATAL) << "Error in GSP: both animal and plant model are disabled.";

  if (m_saveYears.back() > simulDuration)  LOG(FATAL) << "Error in GSP: at least one save time was larger than simulation runtime.";

  LOG(DEBUG) << "***done.";
}
