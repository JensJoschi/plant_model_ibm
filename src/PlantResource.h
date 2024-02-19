/*------------------------------------------------------------------------------
Copyright (C)  2022 - present  Studio Animal-Aided Design

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
 * \file p_PlantResource.h
 * \brief plant resources
 * \details 
 * \author Jens Joschinski
 * \version 1.0
 */

#ifndef PLANTRESOURCE_H
#define PLANTRESOURCE_H

#include "ResourceAlloc.h"
/** @cond */
#include <vector>
/** @endcond */

/**
 * \brief describes how much resource is spent on seeds and reproduction, respectively.
*/
struct Allocations{ 
        int seeds;
        int biomass;
};


/**
 * \brief Plant resources
 * \details
 * This classs converts light into resources (carbon storage) and saves the resources for a plant.
 * - a carbon storage pool of potentially infinite size keeps track of the plant resources. This storage is independent of the plant's age or size.
 * - Light is converted into resources, according to a fixed conversion efficiency.
 * - The plant pays maintenance costs for existing biomass, reducing the resource pool accordingly. The maintenance costs are a fixed fraction of the existing biomass.
 * - if resources are available, the plant will invest in seeds and reproduction, according to a fixed allocation.
 * - if resources go to zero, the resources are considered critical (which may cause the plant to die)
 * The various parameters (conversion efficiency, allocation and maintenance costs) _should_ follow allometric relationships with other PFG attributes, or be otherwise
 * derived from publications and data, but currently they are fixed PFG attributes.
 */
class PlantResource{
    public:
    PlantResource(const ResourceAlloc* const resAlloc_ptr);
    ~PlantResource();

    /**
     * \brief update plant resources
     * \details incoming light is converted into resources, using information from the ResourceAlloc class
     * (conversion efficiency etc).
     * \param light incoming light that is to be converted into resources
     * @return int current resource state
     */
    int updateResource(const int light);

    /**
     * \brief invest in seeds and reproduction
     * \details the plant invests in seeds and reproduction. 
     * 1. the plant pays maintenance costs for existing biomass, reducing the resource pool accordingly.
     * 2. the plant uses the remaining resources (if any) to invest in seeds and reproduction, reducing the resource pool accordingly.
     *  The information comes from the ResourceAlloc class. 
     * As the plant grows, the maintenance costs rise. If there are some bad years and the resource pool depletes, the plant may die due to the maintenance costs.
     * @return Allocations[int seeds, int biomass]  how much resource is spent on seeds and reproduction, respectively.
     */
    Allocations allocateResources(int biomass);

    /**
     * \brief assess resources
     * \details if resources are zero or below, the plant may die
     * @return bool true if resources are zero or below
     */
    bool isResourceCritical() const;


    private:
    const ResourceAlloc* const m_resAlloc_ptr;
    int resources;


};



#endif //RESOURCEPOOL_H