//This file is not meant for sharing. If you have received the file in error, please email us immediately at info@ecolopes.org

// --------------------------------------------------------------------------
// TEMPORARY. PLANT MODEL SHOULD NOT INCLUDE JOINT MODEL CODE IN FOLDER SRC. 
// ALSO, THE FUNCTIONS NOT COMMENTED OUT HERE WILL LIKELY MOVE ELSEWHERE.
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// Authors and contributors to this file:
// Victoria Culshaw (functions that are commented out)
// Jens Joschinski (all other functions)
// --------------------------------------------------------------------------


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Header file
// General Functions namespace. These functions are used in multiple classes.

/* SHORT DESCRIPTION:
 * This class contains only static functions that are used by all animal model class objects.
 * There are some print functions I want to place in here once I have time.
*/

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#ifndef GENERALFUNCTIONSCLASS_H
#define GENERALFUNCTIONSCLASS_H

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <math.h>
#include <random>
#include <string>
#include <vector>

class generalFunctions {
  public:

      
template<typename TK>
static std::vector<std::string> extract_keys_vmvc_JJ(TK& input_map) {
  std::vector<std::string> retval;
  for (const auto& [key, value] : input_map) {
    retval.push_back(key);
  }
  return retval;
}

    /**
     * \brief read json file into a json object
     * \details This function reads a file and stores the result in a json object
     * 
     * \param paramSimulFile Filename
     * @return nlohmann::json resulting json object
     */
    static nlohmann::json readJsonFile(const std::string& paramSimulFile);

    /**
     * \brief turn a key into a vector
     * \details This function works with keys of form "(0, 4, 72)", and supports any number of dimensions (usually 3, but can also be 2). 
     * Note that it will break when keys are negative, do not contain the extra space after the comma, or contain any other characters than numbers, commas and spaces.
     * Note that the function will NOT break on all posisble cases, e.g. "(0, 4, 72,)" or "(0,, 0)". It is up to the caller to do proper error checks. 
     */
    static std::vector<int> stringToVector(std::string_view key);
  
};

#endif

