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
 // RFate team (RFATE)
 // JJ: integrated animal and soil parameters and removed fire, alien and drought modules.
 //     changed default constructor to create a useful set of parameters 
 //     added error checking, json integration (ECOLOPES JOINT MODEL)
 // VC: split global params class into subclasses (ECOLOPES JOINT MODEL)
 // JJ: inheritance and rewrite (ECOLOPES JOINT MODEL/EPM)
 // JJ (IBM): adjust to new parameters
 // --------------------------------------------------------------------------

 /*!
 * \file g_GSP_PLANTS.h
 * \brief Global Simulation Parameters of plant model
 * \details  Contains constant setup variables that are read in or set 
 * during initialization and then not changed anymore (e.g. no. of strata: 4).
 * inherits from and complements GSP_BASE
 */

#ifndef GSP_P_H
#define GSP_P_H

#include "g_GSP_BASE.h"
/** @cond */
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
/** @endcond */

/*!
 * \class GSP
 * \brief Global Simulation Parameters Class
 * \details class contains a set of parameters related to the simulation process.
 * Depending on which modules (dispersal, habitat suitability, light, ...) run,
 * some parameters may not be required.
*/
class GSP_PLANTS: public GSP_BASE{
  public:
    explicit GSP_PLANTS(const nlohmann::json& paramSimulFile);
    GSP_PLANTS();
    /*  general parameters*/
    int noStrata;           /*!< Number of height strata */
    float voxelHeight;      /*!< Height of a voxel */
    float voxelArea;        /*!< Area of a voxel */
    int startingPopSize;
    int initialSeeds;
    int seedRain;             /*!< Number of seeds coming in from the region every time step */
    int soilCapacity;
    int lightDistributionFactor; /*!< determines how much the light is distributed among plants when TotalArea > voxelArea. 1 = single plant may take all  */
    float diffusion;              /*!< the percentage of light that is diffused and bypasses the strata */

     /* plant Modules */
    bool doesDisturbance;       /*!< if this is switched off, the plants are unable to react to disturbances */
    bool doesSoilClass;         /*!< if this is switched off, the attribute "soil class" is ignored */
    bool doesSoilDepth;         /*!< if this is switched off, the attribute "soil depth" is ignored */
    bool doesDispersal;         /*!< if this is switched off, the seeds are unable to disperse */
    bool doesRegionalModel;     /*!< allows specifying where each plant group can occur*/



    private:
    /**
     * \brief add plant parameters to the GSP
     * \param configFile input file (json format; may be same as for GSP_BASE)
     * \details This function reads in the plant parameters from the json file and adds them to the GSP
     */
    void addSpecificParams(const nlohmann::json& j);

    /**
     * \brief check GSP entries for consistency
     * \details This function checks all GSP entries against each other and issues an error if an entry is illogical
     * Checks are onl performed on the new plant-specific parameters
     */
    virtual void checkContent() const override;
};
#endif