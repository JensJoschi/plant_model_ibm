//This file is not meant for sharing. If you have received the file in error, please email us immediately at info@ecolopes.org

// --------------------------------------------------------------------------
// TEMPORARY. PLANT MODEL SHOULD NOT INCLUDE JOINT MODEL CODE IN FOLDER SRC. 
// --------------------------------------------------------------------------


 // --------------------------------------------------------------------------
 // Authors and contributors to this file:
 // JJ: Code and most content of this base class (partially based on FATE but heavily altered)
 // VC: splitting into general and submodel classes
 // JJ: inheritance and rewrite
 // --------------------------------------------------------------------------
 
 /*!
 * \file g_GSP_BASE.h
 * \brief Global Simulation Parameters
 * \details  Contains setup variables that are read in or set 
 * during initialization (e.g. Simulation duration = 50 years).
 * The default constructor intializes with a set of reasonable parameters.
 * \version 1.0
 * \date 2023
 */

#ifndef GSPA_H
#define GSPA_H

/** @cond */
#include <nlohmann/json.hpp>
#include <string>
/** @endcond */


/*!
 * \class GSP
 * \brief Global Simulation Parameters Class
 * \details contains settings that are shared throughout the simulation.
*/
class GSP_BASE{
  public:
  explicit GSP_BASE(const std::string& configFile);
  GSP_BASE();
  
  int simulDuration;        /*!< number of iterations (annual time steps) */

  //// Which submodels should we run?
  bool doesSoil;         /*!< Switches off the soil model. Switches off soil formation as well*/
  bool doesPlants;        /*!< If the Plant model is switched off, static plant resource maps need to be provided so the animal model can run  */
  bool doesAnimals;       /*!< Switches the animal model off */
  bool doesManagement;        /*!< if this is switched off,  management has no effect in any submodel*/
  std::vector<int> m_saveYears;   /*!< Save results in each of those years */

  
  private: 
  /**
   * \brief check GSP entries for consistency
   * \details This function checks all GSP entries against each other and issues an error if an entry is illogical
   */
  virtual void checkContent() const;
  
  /**
   * \brief set GSP entries to default values
   * \details Provides a reasonable set of defaults.
   * \note currently not in use.
   */
  virtual void defaultBuild();
};
#endif