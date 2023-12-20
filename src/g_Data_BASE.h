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


/*!
 * \file g_Inputs.h
 * \brief store all input data
 * \details This class is built with a string, indicating a file name of a json file. 
 * The json file (usually filenames.json) contains the names of all input files.
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

#ifndef INPUTS_ALL_H
#define INPUTS_ALL_H

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

    std::string inputDir;                         /*!< path to input files */
    std::string savingDir;                         /*!< Saving directory path */

    std::string animalModelFile;                  /*! path to the animal model file. May be the same as the file used to create *this class */
    std::string plantModelFile;                   /*! path to the plant model file. May be the same as the file used to create *this class */
    std::string soilModelFile;                    /*! path to the soil model file. May be the same as the file used to create *this class */  


    /* Regional model information */
    std::vector<std::string> listPlantFunctionalGroups;  // list of the PFG that may be present on ecolopesLand.
    std::vector<std::string> listAnimalFunctionalGroups; // ... 

    /* Spatial data */
    //the data is stored as maps whose key is usually a coordinate (string) and the values are the data (int, double, string, etc.)
    //The landscape class is a wrapper around the map, but provides additional functionality
    //(read/write, check spatial extent and completeness etc.)
    Landscape<double> keyList;                  /*!< used only to extract the keys of the site (coordinates). Type double so it matches with the other inputs (you may use e.g. shading.json or similar)*/
    Landscape<double> slope;                /*!< Slope, not used yet*/
    Landscape<std::map<std::string, double>> management; /*!< management maps, content e.g. "(0,0)": {"mowing": 14, "fire": 0}, "(0,1)".... */
    //to discuss whether this should be plant-specific or general information. I think it is general because e.g. mowing would simultaneously affect plants and animals
    //even if not implemented in the animal model, we could already store at the correct position
    
  /**
   * \brief check if keys are consistent
   * \details This function checks if the keys of all spatial data inputs are consistent with the keys provided. the provided keys may also be
   * taken from one of the spatial data inputs (e.g. data_BASE.keylist; this would compare the keys from keylist against management).
   * \param keys list of keys to check
   * \param config global simulation parameters, used to check which model components are switched off
   * @return true all keys are consistent
   * @return false errors detected
   */
   virtual bool checkKeys(const std::vector<std::string>& keys, const GSP_BASE& config) const; 

    /**
     * \brief Read a json value and return a Landscape object
     * \details the json file (e.g. filenames.json, test.json) contains multiple filenames. Entries can be of form 
     * "ShadingFile": "shading.json", or "ShadingFile": ["inputs.json", "SHADING"]. In the latter case inputs.json contains 
     * information from various sources (e.g. shading, soil depth), and only SHADING is read from that file.
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

#endif // INPUTS_ALL_H