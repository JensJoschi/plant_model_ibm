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
// cpp file
// General Functions class. These static functions are used in multiple classes.
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
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
#include <charconv>
#include "easylogging++.h"
#include "nlohmann/json.hpp"
#include "a_generalFunctions_vmvc.h"
// #include "a_homeRange_vmvc.h"

using std::string;
using std::vector;
using std::ifstream;
using std::cout;
using std::endl;
using std::ofstream;
using std::ios;
using std::map;

nlohmann::json generalFunctions::readJsonFile(const std::string& paramSimulFile){
  ifstream f(paramSimulFile.c_str());
  if (!f.good()) LOG(FATAL) << "the file " << paramSimulFile << " cannot be opened. Please check!";
  nlohmann::json j = nlohmann::json::parse(f);
  f.close();
  if (j.empty()) LOG(FATAL) << "the file " << paramSimulFile << " is empty. Please check!";
  return j;
}


std::vector<int> generalFunctions::stringToVector(std::string_view key){
  std::vector<int> result;
  assert(key.front() == '(' && key.back() == ')');
  key.remove_prefix(1); //remove brackets
  key.remove_suffix(1);
  assert(std::all_of(key.begin(), key.end(), [](char c){return c == ',' || c== ' ' || std::isdigit(c);})); //asserts that
  //the string only contains digits, space and commas. Does not catch all bugs, e.g. "" or "(4,,3)"

  while (!key.empty()) { //find region before next ", " place as int in result and shorten key accordingly
    size_t commaPos = key.find(',');
    if (commaPos == std::string_view::npos) {
      commaPos = key.size();
    }
    int value;
    std::from_chars(key.data(), key.data() + commaPos, value);
    result.push_back(value);
    if (commaPos != key.size()) {
      key.remove_prefix(commaPos + 2);  //this includes the space after the comma
    } else {
      key.remove_prefix(commaPos);
    }
  }
  return result;
}

