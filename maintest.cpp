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
// Jens Joschinski
// ----------------------------------------------------------------------------

//THIS FILE IS OBVIOUSLY A STUB AND REQUIRES EXPANSION. CURRENTLY ONLY USED TO TEST THE BUILD SYSTEM

#include "easylogging++.h"
#include "p_PlantModel.h"

#ifndef DEFAULT_WD
std::string default_WD = "";
#else
std::string default_WD = DEFAULT_WD;
#endif


int main(int argc, char *argv[]){
    std::string WD = default_WD;
  el::Configurations conf(WD + "/log.conf"); 
  el::Loggers::reconfigureAllLoggers(conf); 

  //............working directory............
  if(!std::filesystem::exists(WD)){
    std::cerr << "Working directory \"" << WD << "\" does not exist. Please provide a valid path as first argument.";
    return 1;
  }
  std::filesystem::current_path(WD);

//create vector of keys
std::vector<std::string> keys{
    "(0, 0, 0)",
    "(0, 0, 1)",
    "(0, 1, 0)",
    "(0, 1, 1)",
    "(0, 2, 0)",
    "(0, 2, 1)",
    
    "(1, 0, 0)",
    "(1, 0, 1)",
    "(1, 1, 0)",
    "(1, 1, 1)",
    "(1, 2, 0)",
    "(1, 2, 1)",
    
    "(2, 0, 0)",
    "(2, 0, 1)",
    "(2, 1, 0)",
    "(2, 1, 1)",
    "(2, 2, 0)",
    "(2, 2, 1)"};

PlantModel P (WD + "/test.json", keys); //Plantmodel shouldnt include a "keys" vector anymore, to remove.
P.initialize(2);
LOG(INFO) << "foo";
// P.getPFGabund();
}