
/*------------------------------------------------------------------------------
Copyright (C)  2022 - present  Studio Animal-Aided Design

This file is part of the INDIVIDUAL-BASED PLANT MODEL.

INDIVIDUAL-BASED PLANT MODEL is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your option) any later version.

INDIVIDUAL-BASED PLANT MODEL is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with INDIVIDUAL-BASED PLANT MODEL.
If not, see <https://www.gnu.org/licenses/>. */

// --------------------------------------------------------------------------
 /* INDIVIDUAL-BASED PLANT MODEL is derived from the ECOLOPES PLANT MODEL, Copyright (C) 2022-present Studio Animal-Aided Design.

 * ECOLOPES PLANT MODEL is derived, modified and extended from FATE, https://github.com/leca-dev/RFate.git/ 
 * Copyright (C) 2021 Isabelle Boulangeat, Damien Georges, Maya Guéguen, Wilfried Thuiller 
 * For contributions to this particular file, see section "Authors and contributors".*/
// --------------------------------------------------------------------------


/* This particular file is derived and altered from the ECOLOPES JOINT MODEL, 
 Copyright (C) 2022 - present Studio Animal-Aided Design and TU Munich

 ECOLOPES JOINT MODEL is based on:
 - the ECOLOPES PLANT MODEL, 2022 - present  Studio Animal-Aided Design
 - the ECOLOPES ANIMAL MODEL, Copyright (C) 2022 - present TU Munich
 - and the ECOLOPES SOIL MODEL,2022 - present  Studio Animal-Aided Design


 // --------------------------------------------------------------------------
 // Authors and contributors to this file:
 // JJ: Code and most content of this base class (partially based on FATE but heavily altered)
 // VC: splitting into general and submodel classes
 // JJ: inheritance and rewrite
 // JJ(IBM): use json objects, removal of outdated capabilities. It is hoped that JOINT model
 // will eventually merge and adopt those changes, and incorporate the new IBM model
 // --------------------------------------------------------------------------
 */
 
 /*!
 * \file g_GSP_BASE.h
 * \brief Global Simulation Parameters
 * \details  Contains setup variables that are read in or set 
 * during initialization (e.g. Simulation duration = 50 years).
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
  explicit GSP_BASE(const nlohmann::json& configFile);
  GSP_BASE();

  int simulDuration;        /*!< number of iterations (annual time steps) */
  std::vector<int> m_saveYears;   /*!< Save results in each of those years */

  private: 
  /**
   * \brief check GSP entries for consistency
   * \details This function checks all GSP entries against each other and issues an error if an entry is illogical
   */
  virtual void checkContent() const;
  
};
#endif