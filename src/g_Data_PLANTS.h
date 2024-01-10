/*------------------------------------------------------------------------------
Copyright (C) 2023 - present Jens Joschinski

This file is part of the ECOLOPES PLANT MODEL.

ECOLOPES PLANT MODEL is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your option) any later version.

ECOLOPES PLANT MODEL is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with ECOLOPES PLANT MODEL. 
If not, see <https://www.gnu.org/licenses/>. */

// --------------------------------------------------------------------------
 /* ECOLOPES PLANT MODEL is derived, modified and extended from FATE, https://github.com/leca-dev/RFate.git/ 
 * Copyright (C) 2021 Isabelle Boulangeat, Damien Georges, Maya Guéguen, Wilfried Thuiller 
 * For contributions to this particular file, see section "Authors and contributors".*/
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// Authors and contributors to this file:
// Jens Joschinski: Code structure and content
// Victoria Culshaw: split class into submodel classes
// JJ Inheritance and restructuring
// --------------------------------------------------------------------------


#ifndef INPUTS_P_H
#define INPUTS_P_H

#include "g_GSP_PLANTS.h"
#include "g_Data_BASE.h"

#include "p_PFGDefs.h"
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
 * This input is currently the PFG definitions,  shading and soil depth. Soil depth is in cm and shading is a percentage
*/
class Data_PLANTS: public Data_BASE{
  public:
    explicit Data_PLANTS(const std::string& paramSimulFile, const GSP_PLANTS& gsp);
    Data_PLANTS() = default;
    PFGDefs PFGDefinitions;                    // plant functional group definition. Contains life history attributes (lifespan etc.)
    Landscape<double> shading;                 // percentage of shade cover in each grid cell;Expected content: dbl, 0-1; 0.0 = sun-exposed, 1.0 = darkness.
    Landscape<int>    soilDepth;               // depth of soil in cm, integers only. Expected int, 0-100 cm

    /**
    * \brief check data coordinates for consistency
    * 
    * \param keys names of all cells in the landscape
    * \param gsp Global Simulation Parameters, contains information which submodels run
    * @return true if all coordinates are consistent, false otherwise
    */
    virtual bool checkKeys(const std::vector<std::string>& keys, const GSP_PLANTS& gsp) const;

  private:
    /**
   * \brief check plant-specific inputs
   * \details checks plant-specific inputs for errors. called by constructor. 
   * the base ctor has already checked the base inputs, so we only need to check the plant-specific inputs here.
   * In particular, the function checks that all shading values are between 0.0 and 1.0, all depth values >= 0, and that 
   * the PFG definitions are consistent with regional model information.
   * 
   * PFG definitions may be a file with all plants that could theoretically occur; sometimes taken from
   * a different project, or using a globally valid all-purpose file
   * listPlantFunctionalGroups, on the other hand, is a subset (up to 100%) of the PFGDefinitions which occurs on site.
   * Accordingly, PFGDefs may contain more PFGs than listPlantFunctionalGroups, but all PFGs in listPlantFunctionalGroups
   * must be contained in PFGDefs.
   * \param gsp the GSP_PLANT version of global parameters. 
   */
    virtual void checkContent(const GSP_PLANTS& gsp) const;
};


#endif // INPUTS_P_H