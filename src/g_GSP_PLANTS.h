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
 // RFate team
 // JJ: integrated animal and soil parameters and removed fire, alien and drought modules.
 //     changed default constructor to create a useful set of parameters
 //     added error checking, json integration
 // VC: split global params class into subclasses
 // JJ: inheritance and rewrite
 //--------------------------------------------------------------------------

 /*!
 * \file g_GSP_PLANTS.h
 * \brief Global Simulation Parameters of plant model
 * \details  Contains constant setup variables that are read in or set 
 * during initialization and then not changed anymore (e.g. no. of strata: 4).
 * inherits from and complements GSP_BASE
 * \version 1.0
 * \date 2020
 */

#ifndef GSP_P_H
#define GSP_P_H

#include "g_GSP_BASE.h"
/** @cond */
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <string>
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
    explicit GSP_PLANTS(const std::string& paramSimulFile);
    GSP_PLANTS();

     /* plant Modules activation */
    // bool m_DoShading;  /*!< if this is switched off, plants do not shade each other */ This one is a bit difficult to implement
      bool doesNeighbourinfluence; /*!< if this is switched off, light passes at 90° to a stratum (standard FATE-HD) rather than at an angle */
      bool doesShadingPercentages; /*!< if this is switched off, cells will not differ in shading percentages*/
    bool doesHabSuitability;    /*!< if this is switched off, all plants can live everywhere */
      bool doesSoilClass;         /*!< if this is switched off, the attribute "soil class" is ignored */
      bool doesSoilDepth;         /*!< if this is switched off, the attribute "soil depth" is ignored */
    bool doesPlantDispersal;    /*!< if this is switched off, the plants are unable to disperse */
    bool doesDisturbance;       /*!< if this is switched off, the plants are unable to react to disturbances */

    /*  global parameters*/
    int potentialFecundity; /*!< Potential Fecundity of mature plants (maximum value of seeds produced in optimal conditions) */
    int noStrata;           /*!< Number of height strata */
    std::vector<int> strataHeight; /*!< Height of each strata */
    int maxAbundLow;        /*!< Maximum abundance or space a PFG can occupy : low value */
    int maxAbundMedium;     /*!< Maximum abundance or space a PFG can occupy : medium value */
    int maxAbundHigh;       /*!< Maximum abundance or space a PFG can occupy : high value */

    //dispersal
    int seedInput;       /*!< Number of seeds introduced during seeding */

    // shading
    int lightThreshLow;    /*!< Threshold to transform PFG abundances into Low light resources */
    int lightThreshMedium; /*!< Threshold to transform PFG abundances into Medium light resources */
    int lightAngle;        /*!< Angle of the sun in degrees, range 0-90 (90 = zenith) */
    //disturbance



    private:
    /**
     * \brief add plant parameters to the GSP
     * \param configFile input file (json format; may be same as for GSP_BASE)
     * \details This function reads in the plant parameters from the json file and adds them to the GSP
     */
    void addSpecificParams(const std::string& configFile);

    /**
     * \brief check GSP entries for consistency
     * \details This function checks all GSP entries against each other and issues an error if an entry is illogical
     * Checks are onl performed on the new plant-specific parameters
     */
    virtual void checkContent() const override;

    public:
    /**
     * \brief provides useful defaults for all parameters
     * \details This function provides defaults for plant-specific parameters only.
     * \note currently not in use. Set to public so unit tests can use it though.
     */
    virtual void defaultBuild() override;
};
#endif