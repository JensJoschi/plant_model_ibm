
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

// --------------------------------------------------------------------------
// Authors and contributors to this file:
// Jens Joschinski: Code structure and content (ECOLOPES JOINT MODEL)
// Victoria Culshaw: split class into submodel classes (ECOLOPES JOINT MODEL)
// JJ Inheritance and restructuring (ECOLOPES JOINT MODEL/EPM)
// JJ update to new model (IBM)
// --------------------------------------------------------------------------


#ifndef INPUTS_P_H
#define INPUTS_P_H

#include "GSP_PLANTS.h"
#include "g_Data_BASE.h"

#include "Landscape.h"

/** @cond */
#include <string>
#include <vector>
#include "nlohmann/json.hpp"
/** @endcond */

/*! 
 * \class Inputs
 * \brief contains plant-specific input
 * \details derives from Data_BASE and complements it by plant-specific input
*/
class Data_PLANTS: public Data_BASE{
  public:
    explicit Data_PLANTS(const nlohmann::json& j, const GSP_PLANTS& gsp);
    Data_PLANTS() = default;

    std::string logger = "plantlog.conf";       // name of the logger configuration file
  
    Landscape<double> light;                  // amount of light in each voxel cell;
    Landscape<int>    soilDepth;               // depth of soil in cm, integers only. Expected int, 0-100 cm
    Landscape<std::string> soilClass;          // soil class as string, describes what "kind" of soil is available in each cell (e.g. "sand", "unstructured_acidic_loam").
    Landscape<std::map<std::string, double>> management; // management information for each cell. Contains information on grazing, mowing, etc.
    Landscape<std::vector<std::string>> allowedTypes; // enumerates which plants are allowed in each cell.
    /**
    * \brief check data coordinates for consistency
    * 
    * \param keys names of all cells in the landscape
    * \param gsp Global Simulation Parameters, contains information which submodels run
    * @return true if all coordinates are consistent, false otherwise
    */
    virtual bool checkKeys(const GSP_PLANTS& gsp) const;

  private:
    /**
   * \brief check plant-specific inputs
   * \details checks plant-specific inputs for errors. called by constructor. 
   * the base ctor has already checked the base inputs, so we only need to check the plant-specific inputs here.
   * 
   * \param gsp the GSP_PLANT version of global parameters. 
   */
    virtual void checkContent(const GSP_PLANTS& gsp) const;
};


#endif // INPUTS_P_H