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
 // Jens Joschinski (IBM); 
 // --------------------------------------------------------------------------


/*!
 * \file p_rsourcePool.h
 * \brief plant resources
 * \details 
 */

#ifndef RESOURCEALLOC_H
#define RESOURCEALLOC_H

/** @cond */
#include "nlohmann/json.hpp"
/** @endcond */


/**
 * \brief Resource allocation information
 * \details
 * This class holds information about the allocation of resources to growth and reproduction.
 * \note
 * This class is on purpose inaccessible except by one specialized class (SeedPool). Makes it easier to maintain the code and to add new features.
 */
class ResourceAlloc{
    friend class PlantResource;

    public: 
    /**
     * \brief Construct a new Resource Alloc object
     * \details The json file must contain the following entries:
     * - "conversionEfficiency": The photosynthetic efficiency of the plant.
     * - "maintenanceCosts": The maintenance costs of the plant.
     * - "seedAllocation": The allocation of resources to seed production.
     * - "biomassAllocation": The allocation of resources to biomass production.
     * - "maxBiomass": The maximum biomass that can be reached.
     * \param traits 
     */
    ResourceAlloc(const nlohmann::json& traits);

    private:
    float conversionEfficiency;  /*The photosynthetic efficiency of the plant. Typical values are in the order of 0.005 (https://en.wikipedia.org/wiki/Photosynthetic_efficiency;*/
    float maintenanceCosts;
    float seedAllocation;
    float biomassAllocation;
    int maxBiomass;                  /*!< Maximum biomass that can be reached */
    void check();
};
#endif //RESOURCEALLOC_H
