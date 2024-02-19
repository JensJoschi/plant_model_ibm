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
 // Jens Joschinski: readJsonFile function
 // Victoria Culshaw: 
 // --------------------------------------------------------------------------


#ifndef GENERALFUNCTIONSCLASS_H
#define GENERALFUNCTIONSCLASS_H

/** @cond */
#include "nlohmann/json.hpp"
#include <string>
#include <fstream>
#include "easylogging++.h"
/** @endcond */

namespace generalFunctions {
  /**
   * \brief read json file into a json object
   * \details This function reads a file and stores the result in a json object
   * 
   * \param paramSimulFile Filename
   * @return nlohmann::json resulting json object
   */
  static nlohmann::json readJsonFile(const std::string& paramSimulFile){
    std::ifstream f(paramSimulFile.c_str());
    if (!f.good()) LOG(FATAL) << "the file " << paramSimulFile << " cannot be opened. Please check!";
    nlohmann::json j = nlohmann::json::parse(f);
    f.close();
    if (j.empty()) LOG(FATAL) << "the file " << paramSimulFile << " is empty. Please check!";
    return j;
  }
}

#endif

