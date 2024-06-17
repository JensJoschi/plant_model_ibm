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

// ----------------------------------------------------------------------------
// Authors and contributors to this file:
// Jens Joschinski (EPM)
// ----------------------------------------------------------------------------

/*! \mainpage Plant Model
 *
 * \section intro_sec Introduction

 * For further information about the model, click on README under the table of contents on the left.
 *
 */

#include "g_Inputs.h"
#include "g_Data_BASE.h"
#include "g_GSP_BASE.h"
#include "PlantModel.h"
#include "Community.h"
#include "Traits.h"
#include "LifeHistory.h"
#include "generalFunctions.h"

/** @cond */
#include <string>
#include <random>
#include "rng.h"
#include <filesystem>
#include "easylogging++.h"
#include "config.h"
/** @endcond */

//a default working directory can be set during compilation, see also CmakeLists.txt.
//this default WD can be overwritten by providing a command line argument to the executable.
#ifndef DEFAULT_WD
std::string default_WD = "";
#else
std::string default_WD = DEFAULT_WD;
#endif


INITIALIZE_EASYLOGGINGPP
/**
 * @file main.cpp
 * @brief This file contains the main function that initializes the program and runs the model.
 */



int main(int argc, char *argv[]){
std::cout << "Plant model version " << VERSION << std::endl;
  //............command line arguments............
  std::string WD = default_WD;
  std::string input_file = "test.json";
  bool fix_RNG = false;
  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];
    if ((arg == "-w") && (i + 1 < argc)) { // -w
      WD = argv[++i];
      std::cout << "Working directory set to \"" << WD << "\".";
    } else if ((arg == "-f") && (i + 1 < argc)) { // -f
      input_file = argv[++i];
      std::cout << "Input file set to" << input_file;
    } else if (arg == "-r") { // -r
        fix_RNG = true;
        std::cout << "RNG fixed. This is not recommended for production runs";
    } else if (arg == "-h") { // -h
      std::cout << "Please use -w to set working directory and -f to set input file.";
      return 0;
    } else {
      std::cerr << "Unknown argument" << arg << "Please use -w to set working directory and -f to set input file.";
      return 1;
    }
  }

  //............working directory............
  if(!std::filesystem::exists(WD)){
    std::cerr << "Working directory \"" << WD << "\" does not exist. Please provide a valid path as first argument.";
    return 1;
  }
  std::filesystem::current_path(WD);

  //............logging............
  el::Configurations conf(WD + "log.conf"); 
  el::Loggers::reconfigureAllLoggers(conf); 

  LOG(INFO) << "Logger initialized.";
  LOG(INFO) << "Working directory set to \"" << WD << "\".";
  LOG(INFO) << "Input file set to \"" << WD+input_file << "\".";
  LOG(INFO) << "Plant model version " << VERSION << ".";
  LOG(INFO) << SECTIONBREAK << SECTIONBREAK << SECTIONBREAK;

  //............model............
  GSP_PLANTS config{generalFunctions::readJsonFile(WD+input_file)};
  Data_PLANTS data{generalFunctions::readJsonFile(WD+input_file), config};
  

  LOG(INFO) << SECTIONBREAK << "CREATE PLANT MODEL";
  nlohmann::json g {generalFunctions::readJsonFile(WD+input_file)};
  nlohmann::json v {generalFunctions::readJsonFile(WD+input_file)};

    //FOR TESTING
  nlohmann::json t {generalFunctions::readJsonFile(WD+"building2/onetree.json")};


  PlantModel Plantmodel(g, v, t, "", fix_RNG);

  if (config.simulDuration == 0){ LOG(INFO) << "Warning: model only initialized, not running, because simulDuration == 0"; return 0; }
  else LOG(INFO) << SECTIONBREAK << "RUN MODEL";

  for(int tplus1=1; tplus1< config.simulDuration; ++tplus1){
    Plantmodel.TPlusOne();
    Plantmodel.save(tplus1);
    Plantmodel.saveArea(tplus1);
  }

  //............end............
  LOG(INFO) << SECTIONBREAK << "*************Finished.************";
}