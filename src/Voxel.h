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
 * \file Voxel.h
 * \brief voxel Cell with soil,community and light
 */

#ifndef VOXEL_H
#define VOXEL_H

#include "Community.h"
#include "Illumination.h"
#include "Soil.h"
#include "Landscape.h"

/** @cond */
#include <memory>
#include <map>
#include <string>
#include <vector>
#include <string_view>
#include <nlohmann/json.hpp>
/** @endcond */


/*!
 * \class Voxel
 * \brief Voxel cell
 * \details 
 * A Voxel is a cell in the model. It contains a community of plants, soil and light. Plants
 * have a home cell in which they interact with e.g. soil and disturbances, but they can also grow into
 * neighbouring cells
 * */
class Voxel{
    public:
    /**
     * \brief Construct a new Voxel object
     * \details 
     * The constructor creates soil, a plant community and illumination.
     * capacity, depth, name belong to the (initial) soil; MaxIndividuals and 
     * InitialSeeds to the community; LightDistributionFactor and diffusion to illumination.
     * The traits define which type of community may be created in this voxel. 
     */
    Voxel(int capacity, int depth, const std::string& name, 
        std::map<std::string_view, const Traits*> traits, int maxIndividuals, int initialSeeds,
        const std::vector<Stratum>& voxel, int lightDistributionFactor, float diffusion);
     
    ~Voxel();
    Voxel(int capacity, int depth, const std::string& name, 
        std::map<std::string_view, const Traits*> traits, int maxIndividuals, int initialSeeds,
        const std::vector<Stratum>& voxel,int lightDistributionFactor,float diffusion,
        const nlohmann::json& individuals);

    //delegating functions: Illumination
    void distributeArea();
    void sendLightBeam(int light);
    void addNeighbours(const std::vector<Voxel*>& neighbours);

    //delegating functions: Community
    void rainSeeds(const std::map<std::string_view, int>& seeds);
    std::map<std::string, int> age();
    int getCount(std::string_view type = "") const;
    int getCount (int from, int to, std::string_view type = "") const;
    float getBiomass(std::string_view type = "") const; //age 0 excluded
    float getPlantCover() const;

    private: //warning: order is important, soil has to be initialized first
    std::shared_ptr<Soil> m_soil;
    Community m_community;
    Illumination m_illumination;
};

#endif // VOXEL_H