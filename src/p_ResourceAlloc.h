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

 // ----------------------------------------------------------------------------
 // Authors and contributors to this file:
 // Jens Joschinski
 // ----------------------------------------------------------------------------

 // ----------------------------------------------------------------------------
 //THIS FILE IS FOR A NEW IMPLEMENTATION OF PLANT MODEL AND NOT BEING USED YET.
 // ----------------------------------------------------------------------------

/*!
 * \file p_rsourcePool.h
 * \brief plant resources
 * \details 
 * \author Jens Joschinski
 * \version 1.0
 */

#ifndef RESOURCEALLOC_H
#define RESOURCEALLOC_H


/**
 * \brief Resource allocation information
 * \details
 * This class holds information about the allocation of resources to growth and reproduction. Currently these are just some fixed attributes, 
 *  but ultimately the class may consist of the following information:
 * - allocation to growth vs to seeds vs to storage (this replaces the potFecundity variable in PFG attributes)
 * - size of storage system (roots)
 * - maintenance costs of existing biomass 
 * - minimal yearly investment(even if no resource is available, plants need to keep growing)
 * - max yearly investment (if light is abundant, plants can still not grow infinitely fast)
 * - light conversion efficiency (how much light is needed to produce 1 biomass unit)
 
 * Further notes: 
 *  max yearly investment and allocation together determine growth rate;
 *  replaces immSize and potFecundity in PFG attributes;
 *  immature plants shouldalways allocate 100% to growth.
 *  reading of model papers required to figure out exact variables and allometric relationships with other PFG attributes
 */
class ResourceAlloc{
    friend class PlantResource;
    public: 
    ResourceAlloc();
    ResourceAlloc(float, float, float, float, float);
    ~ResourceAlloc();
    private:
    float conversionEfficiency;
    float maintenanceCosts;
    float seedAllocation;
    float biomassAllocation;
    float maxInvestment;
};
#endif //RESOURCEALLOC_H
