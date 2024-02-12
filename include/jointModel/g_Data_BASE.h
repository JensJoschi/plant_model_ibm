/* Copyright (C) 2023 - present ???
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
 - the ECOLOPES PLANT MODEL, Copyright (C) 2023 ???
 - the ECOLOPES ANIMAL MODEL, Copyright (C) 2023 ???
 - and the ECOLOPES SOIL MODEL, Copyright (C) 2023 ???

*/
 // --------------------------------------------------------------------------
 // Authors and contributors to this file:
 // JJ: Code and most content of this base class (partially based on FATE but heavily altered)
 // VC: splitting into general and submodel classes
 // JJ: inheritance and rewrite
 // --------------------------------------------------------------------------
 

/*!
 * \file Data_BASE.h
 * \brief store input data
 * \note when the class is built, it reads in the information e.g. from the regional model
 * (listplantfunctionalgroups). This class can, however,not assert that the information is correct,
 * because it does not have access to the definitions yet. So, asking the inputs_all class for data
 * is inherently problematic. Maybe this class should be private after all? only accessed by ecolopesland,
 * and ecolopesland is responsible for cross-checking with the other inputs before sending data to the other 
 * classes? This way, the animal and plant models also require no other class when they run as standalones.
 * \version 1.0
 * \date 2023-03-28
 * \author Jens Joschinski
 */

#ifndef INPUTS_B_H
#define INPUTS_B_H

#include "g_GSP_BASE.h"
#include "Landscape.h"


/** @cond */
#include <string>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include <fstream>
#include "easylogging++.h"
/** @endcond */

/*! 
 * \class Inputs
 * \brief contains all input
 * also checks input for errors
*/
class Data_BASE{
  public:
    explicit Data_BASE(const std::string& paramSimulFile, const GSP_BASE& gsp);
    Data_BASE() = default;

    std::string inputDir;        /*!< path to input files */
    std::string savingDir;       /*!< Saving directory path */

    /* Regional model information */
    //possibly move to Data_ECOLOPES
    std::vector<std::string> listPlantFunctionalGroups;  // list of the PFG that may be present on ecolopesLand.

    /* Spatial data */
    //the data is stored as maps (Landscape class) whose key is 
    //usually a coordinate (string) and the values are the data (int, double, string, etc.)
    Landscape<double> keyList;    /*!< used only to extract the keys of the site (coordinates). */
    
  /**
   * \brief check if keys are consistent
   * \details This function checks if the keys of all spatial data inputs are consistent . 
   * \note not very sensible yet because there is only one dataset; but inherited classes may have more
   * \param config global simulation parameters, used to check which model components are switched off
   * @return true all keys are consistent
   * @return false errors detected
   */
   virtual bool checkKeys( const GSP_BASE& config) const; 

    /**
     * \brief Read a json value and return a Landscape object
     * \details the json file (e.g. filenames.json, test.json) contains multiple filenames. Entries can be of form 
     * "ShadingFile": "shading.json", or "ShadingFile": ["inputs.json", "SHADING"]. In the latter case inputs.json contains 
     * information from various sources (e.g. shading, soil depth), and only SHADING is read from that file.
     * \note not very sensible yet because there is only one dataset; but inherited classes may have more
     * 
     * \tparam T usually double or int
     * \param j the json object, containing s
     * \param s key to read (e.g. "filename: "foo.json"; here s would be "filename")
     */
    template <typename T>
    Landscape<T> readFile(const nlohmann::json& j, const std::string& s, std::string inpdir){
      if(j.at(s).is_string()){
          std::string fileString  = j.at(s);
       return Landscape<T>(inpdir + fileString);
      } else if(j.at(s).is_array()){
        std::vector<std::string> fileVector = j.at(s);
        if (fileVector.size() != 2) {LOG(FATAL) << "Problem with file " << s << ". Too many arguments (" << fileVector.size() <<").";}
        return Landscape<T>(inpdir + fileVector[0], fileVector[1]);
      }
      else{
       LOG(FATAL) << "ERROR: " << s << " must be either a string or an array of strings";
       return Landscape<T>();
      }

    }
  
  private:
  /**
   * \brief check input for errors
   * \details This function checks the input for errors. It is called by the constructor.
   * Uses the global simulation parameters to check for inconsistencies. In this (base) class, this makes no sense,
   * but derived classes may e.g. check if a submodule is turned on or off before checking the submodule-specific parameters
   * 
   * \param gsp global simulation parameters
   */
  virtual void checkContent(const GSP_BASE& gsp) const;
};

#endif // INPUTS_B_H