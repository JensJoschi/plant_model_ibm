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

 // ----------------------------------------------------------------------------
 // Authors and contributors to this file:
 // Jens Joschinski (IBM)
 // ----------------------------------------------------------------------------


/*!
 * \file p_PlantResource.h
 * \brief plant resources
 * \details 
 */

#ifndef PLANTRESOURCE_H
#define PLANTRESOURCE_H

#include "ResourceAlloc.h"
/** @cond */
#include <vector>
#include <utility>
#include <nlohmann/json.hpp>
/** @endcond */

/**
 * \brief Plant resources
 * \details
 * Takes care of collecting light, converting it into resources, and spending resources on growth and reproduction.
 *  * - The plant converts light into a resource. One light unit (in watt/m2) is converted into conversionEfficency resource units
 *    (e.g. carbohydrates in g). Typical values are in the order of 0.005 (https://en.wikipedia.org/wiki/Photosynthetic_efficiency;
 *    ~5% efficiency, but this includes direct above-and belowground biomass gain; storing as starch incurs further cost)
 * - the plant needs to pay an annual maintenance cost of maintenanceCosts * biomass
 * - the plant can allocate the fraction seedAllocation of the resources to reproduction and up to biomassAllocation to biomass.
 * - biomass growth follows a sqrt function, i.e. the growth rate decelerates as the biomass approahces maxBiomass; this limits 
 *   biomass even if enough resources are available.
 * Ultimately this class may also contain
 * - capacity of storage system (roots)
 * - minimal yearly investment(even if no resource is available, plants need to keep growing)
 * - possibly max annual investment (otherwise small plants grow very much in first year)
 * - conversion from resource to biomass. Currently one resource makes one biomass unit
 
 * Further notes: 
 *  immature plants shouldalways allocate 100% to growth and 0 to seeds
 *  reading of model papers required to figure out exact variables and allometric relationships with other PFG attributes
 * https://doi.org/10.1016/S0304-4238(98)00083-1
 * 
 * \note plants can store an infinite amount of resources. 
 * \note resource is arbitrarily initiated with 100 biomass. Prevents plants from being too small to grow (resource allocation is proportional to biomass)
 */
class PlantResource{
    friend class ResourceTest_updateResource_Test;
    public:
    PlantResource(const ResourceAlloc* const resAlloc_ptr);
    PlantResource(const ResourceAlloc* const resAlloc_ptr, nlohmann::json j);
    ~PlantResource();
    /**
     * \brief update plant resources
     * \details incoming light is converted into resources, using information from the ResourceAlloc class
     * (conversion efficiency etc).
     * \param light incoming light that is to be converted into resources
     * \param soilIsSuitable if the soil is suitable for the plant
     */
    void updateResource(int light, bool soilIsSuitable = true);

    /**
     * \brief invest in seeds and reproduction
     * \details the plant invests in seeds and reproduction. 
     * 1. the plant pays maintenance costs for existing biomass, reducing the resource pool accordingly.
     * 2. the plant uses the remaining resources (if any) to invest in seeds and reproduction, reducing the resource pool accordingly.
     *  The information comes from the ResourceAlloc class. 
     * As the plant grows, the maintenance costs rise. If there are some bad years and the resource pool depletes, the plant may die due to the maintenance costs.
     * @return   if plant survives, and how many new seeds were produced
     */
    std::pair<bool, int> allocateResources();

    /**
     * \brief reduce biomass by a certain amount
     * \details removes a certain amount of biomass. If the plant is smaller than the disturbance, the remaining 
     * amount of disturbance is returned
     * \param amount disturbance 
     * @return non-consumed disturbance
     */
    float disturb( float amount);

    /**
     * \brief reduce saved resources by a certain amount
     * \details removes a certain amount of resources. If the plant has fewer resources than required, the remaining amount
     * of depletion is returned. 
     * \param amount of resource depleted 
     * @return  amount of resource that could not be depleted
     */
    float depleteResources(float amount); 

    /**
     * \brief Get the current biomass
     */
    float getBiomass() const;

    private:
    const ResourceAlloc* const m_resAlloc_ptr;
    float resources;
    float biomass;

    /**
     * \brief check of input parameters
     * \details if a json with parameters is provided, this function checks the consistency with Resource Allocation attributes.
     */
    void check() const;
};



#endif //RESOURCEPOOL_H