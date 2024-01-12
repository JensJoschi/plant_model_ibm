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
#include "p_ResourceAlloc.h"
#include "p_PlantResource.h"  

#include <cassert>

#ifndef WorkDir
std::string defWD = "";
#else
std::string defWD = WorkDir;
#endif


int main(int argc, char *argv[]){
    std::string WD = defWD;

  //............working directory............
  if(!std::filesystem::exists(WD)){
    std::cerr << "Working directory \"" << WD << "\" does not exist. Please provide a valid path as first argument.";
    return 1;
  }
  std::filesystem::current_path(WD);

    const ResourceAlloc allocation;  //normally this is part of the PFG definition
  PlantResource pr(&allocation);
  assert(pr.isResourceCritical());
LOG(INFO) << "Test passed";
}