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


 // ----------------------------------------------------------------------------
 // Authors and contributors to this file:
 // Jens Joschinski
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
#include "p_PlantModel.h"

/** @cond */
#include <string>
#include <random>
#include "rng.h"
#include <filesystem>
#include "easylogging++.h"
/** @endcond */

                      typedef Inputs<GSP_BASE, Data_BASE> Inputs_G;  //temporary

//a default working directory can be set during compilation, see also CmakeLists.txt.
//this default WD can be overwritten by providing a command line argument to the executable.
#ifndef DEFAULT_WD
std::string default_WD = "";
#else
std::string default_WD = DEFAULT_WD;
#endif

/**
 * @file main.cpp
 * @brief This file contains the main function that initializes the program and runs the model.
 * 
 * The main function reads command line arguments, sets the working directory, initializes logging, 
 * sets the random number generator, reads the input file, creates an EcolopesLand object, runs the model, 
 * saves the output files, combines the output files, and ends the program.
 */




/*============================================================================
Main-------------------------------------------------------------------------
---------------------------------------------------------------------------*/
int main(int argc, char *argv[]){

  //............command line arguments............
  std::string WD = default_WD;
  std::string input_file = "filenames.json";
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
  el::Configurations conf(WD + "/log.conf"); 
  el::Configurations plants_conf(WD + "/log_plants.conf"); 
  el::Loggers::getLogger("PLANTS");
  el::Loggers::reconfigureAllLoggers(conf); 
  el::Loggers::reconfigureLogger("PLANTS", plants_conf);
  el::Loggers::addFlag(el::LoggingFlag::MultiLoggerSupport);
  //typical logger use is LOG(INFO) << "text";  to write into the default logger;
  //or CLOG(INFO, "PLANTS") << "text"; to write into one specific logger; 
  //or (CLOG, INFO, "PLANTS", "default") << "text"; to write into two or more loggers at once.
  //LOGGERS is a macro that expands to "PLANTS", "default" and is used in the last case.
  //SECTIONBREAK is another macro that expands to "------------------------". Both are defined in easylogging.h, so they are automatically included everywhere.
  CLOG(INFO, LOGGERS) << SECTIONBREAK << SECTIONBREAK << SECTIONBREAK;
  CLOG(INFO, LOGGERS) << "Logger initialized.";
  CLOG(INFO, LOGGERS) << "Working directory set to \"" << WD << "\".";
  CLOG(INFO, LOGGERS) << "Input file set to \"" << WD+input_file << "\".";

  //............random number generator............
  if (fix_RNG){
    CLOG(INFO, LOGGERS) << "fix RNG with seed 2230";
    RNGs::mersenne = std::mt19937{2230};
  }


  //............model............
                                  const Inputs_P inputs (WD + input_file); //temporary
                                std::vector<std::string> keys  = inputs.data.keyList.getKeys(); //temporary

  CLOG(INFO, LOGGERS) << SECTIONBREAK << "CREATE PLANT MODEL";
  PlantModel Plantmodel(WD+input_file, keys);
  int tplus1 = 0;
  int iterateOverSaveYears = 0; 

  Plantmodel.initialize(5);  

  if (inputs.config.m_saveYears[iterateOverSaveYears] == 0){
    LOG(INFO) << "SAVING";
    //save(iterateOverSaveYears);
    //createSummary(iterateOverSaveYears);
    ++iterateOverSaveYears;
  }
  if (inputs.config.simulDuration == 0){ CLOG(INFO, LOGGERS) << "Warning: model only initialized, not running, because simulDuration == 0"; return 0; }
  else CLOG(INFO, LOGGERS) << SECTIONBREAK << "RUN MODEL";

  for(tplus1=1; tplus1< inputs.config.simulDuration; ++tplus1){
    Plantmodel.TPlusOne_JJ();
    if (inputs.config.m_saveYears[iterateOverSaveYears] == tplus1){
      LOG(INFO) << "SAVING";
      
      //call getPFGabund here!

      //save function required
      ++iterateOverSaveYears;
      }
  }

  //............outputs............
  LOG(INFO) << SECTIONBREAK << "COMBINE OUTPUT FILES";
 //combine function required

  //............end............
  CLOG(INFO, LOGGERS) << SECTIONBREAK << "*************Finished.************";
}