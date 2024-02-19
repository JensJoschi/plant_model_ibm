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

#ifndef GSPB_H
#define GSPB_H

/** @cond */
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
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