//This file is not meant for sharing. If you have received the file in error, please email us immediately at info@ecolopes.org

// --------------------------------------------------------------------------
// TEMPORARY. PLANT MODEL SHOULD NOT INCLUDE JOINT MODEL CODE IN FOLDER SRC. 
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// Authors and contributors to this file:
// Jens Joschinski: initial class
// Victoria Culshaw: split class into submodel classes
// JJ: class inheritance and code restructuring
// --------------------------------------------------------------------------

#include "g_Data_BASE.h"
#include "g_GSP_BASE.h"

/** @cond */
#include <string>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include <fstream>
#include "easylogging++.h"
/** @endcond */


#include "a_generalFunctions_vmvc.h"
#include "Landscape.h"

Data_BASE::Data_BASE(const std::string& paramSimulFile, const GSP_BASE& gsp):
  animalModelFile(paramSimulFile), plantModelFile(paramSimulFile), soilModelFile(paramSimulFile){
  LOG(DEBUG) << SUBSECTIONBREAK << "Reading shared data from json file.";
  LOG(INFO) << "Reading shared data from json file.";

  //----------------------------------------------------------------------------
  LOG(DEBUG) << "--Data sources";
  nlohmann::json j = generalFunctions::readJsonFile(paramSimulFile);

  try{inputDir = j.at("InputDir");}
    catch(nlohmann::json::out_of_range) { LOG(WARNING) << "InputDir not found. Set to 0-INPUT/"; inputDir = "0-INPUT/";}
  try{savingDir = j.at("SaveDir");}
    catch(nlohmann::json::out_of_range) {  LOG(WARNING) << "Saving dir not found. Using 0-RESULTS/"; savingDir = "0-RESULTS/";}

  try{plantModelFile = j.at("PlantInputs");}
    catch(nlohmann::json::out_of_range) {  LOG(WARNING) << "flag PlantInputs  not found, using default(" << plantModelFile << ")";}
  try{animalModelFile = j.at("AnimalInputs");}
    catch(nlohmann::json::out_of_range) {  LOG(WARNING) << "flag AnimalInputs not found, using default(" << animalModelFile << ")";}
  try{soilModelFile = j.at("SoilInputs");}
    catch(nlohmann::json::out_of_range) {  LOG(WARNING) << "flag SoilInputs   not found, using default(" << soilModelFile << ")";}
  
  //----------------------------------------------------------------------------
  LOG(DEBUG) << "--regional model information";
//temporary, in reality it should check if regional model has been provided
//ideally it should also compare against PFG/AFG/soil definitions, but it does not know them 
  try{listPlantFunctionalGroups = j.at("listPlantFunctionalGroups");}
    catch(nlohmann::json::out_of_range) {  LOG(FATAL) << "listPlantFunctionalGroups not found";}
  
  try{listAnimalFunctionalGroups = j.at("listAnimalFunctionalGroups");}
    catch(nlohmann::json::out_of_range) {  LOG(FATAL) << "listAnimalFunctionalGroups not found";}
  
  std::sort(listPlantFunctionalGroups.begin(), listPlantFunctionalGroups.end());
  std::sort(listAnimalFunctionalGroups.begin(), listAnimalFunctionalGroups.end());



  //----------------------------------------------------------------------------------------
  LOG(DEBUG) << "--Input Data";
  try{keyList = readFile<double>(j, "MaskFile" , inputDir);}
  catch(nlohmann::json::out_of_range) {   
     LOG(FATAL) <<"MaskFile not found"; 
  }


  slope = Landscape<double>();  //to do
    if (gsp.doesManagement){
    try{management = readFile<std::map<std::string, double>>(j, "Management", inputDir);}
    catch(nlohmann::json::out_of_range) { LOG(FATAL) << "Management not found.";}
    }

  checkContent(gsp);
}


void Data_BASE::checkContent(const GSP_BASE& gsp) const{

  LOG(DEBUG) << "--Performing checks";
  std::filesystem::create_directory(inputDir);  //will not create a new one if already present
  std::filesystem::create_directory(savingDir);

  if(gsp.doesManagement){
    LOG(DEBUG) << "Management";
    if(management.getTotncell() == 0) LOG (ERROR) << "map of management wrong";
  }

  LOG(DEBUG) << "--All checks done";
}


bool Data_BASE::checkKeys(const GSP_BASE& config) const{
  if (config.doesManagement && management.getTotncell() != keyList.getKeys().size()) return false;
  else {
    for (const auto& key : keyList.getKeys()){
      if (config.doesManagement && management.count(key) != 1) return false;
      if (keyList.count(key) != 1) return false;
    }
    return true;
  }
}