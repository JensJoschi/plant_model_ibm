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
 * \file Illumination.h
 * \brief Light calculations
 * \details Contains all variables and functions to calculate the light that reaches individuals Plants.
 * Part of the components that make up a Voxel
 */

#ifndef ILLUMINATION_H
#define ILLUMINATION_H

#include "Individual.h"
#include "Community.h"
class Voxel;
class Community;

/** @cond */
#include <vector>
#include <map>
/** @endcond */

struct Stratum{
    float from; /**< The lower bound of the stratum */
    float to;   /**< The upper bound of the stratum */
    float area; /**< The area of the stratum */

     bool operator<(const Stratum& other) const {
        return from < other.from;
    }
};

/*!
 * \class Illumination
 * \brief Light calculations of a Voxel
 * \details 
 * Contains all variables and functions to calculate the light that reaches individuals Plants.
 * The voxel is stratified; light beams that hit the voxel at an 
 * angle of 90 degrees first hit the uppermost strata and plants therein. Any light that is not
 * absorbed by the plants in the uppermost strata is passed on to the next strata, and so on. 
 * Some percentage of incoming light will not hit any plant but instead bypass the stratum to be consumed by next stratum (except for lowest stratum). 
 * The percentage is determined by the diffusion parameter.
 * The remaining light leaving a stratum is homogenized over the full area of the next stratum. This means that
 * the plant shades itself partially but not completely. Example:
 * If a plant covers 2/3 of the uppermost stratum's area, and 1/3 of the area in the next stratum, it receives 66% of the light
 * in the upper stratum. The remaining third of the light gets passed on to the next stratum. There, the plant receives  
 * 33% of the *remaining* light (11%). The remainder(22%) passes through the stratum without hitting the plant. 
 * Without the homogenization the uppermost part would receive 66%, but the lower part would be entirely shaded and receive nothing; 
 * and 33% of the light would be lost.  
 * Plant growth is not directly controlled by the voxel/illumination model. The sum of all plant material (from many small plants) 
 * may be larger than what the voxel can accomodate, or an individual plant may grow larger than the voxel.
 * When a single plant covers an area larger than the voxel Area, its excess area is split into equal parts and distributed
 * to the neighbouring cells. The material from neighbours is accordingly also added to this voxel's area and may also receive light
 * When there is too much material in the voxel (both from plants growing here and spilling over from neighbours), a top layer 
 * of plant material is randomly selected and receives light. The number of draws is a parameter (lightDistributionFactor).
 * This class associates with individuals and is responsible for feeding them with light.
 * \note ECOLOPES PLANT MODEL allows for angular light input, this is not yet implemented. See issue #8
 * */
class Illumination{
    typedef std::pair<std::weak_ptr<Individual>, float> IndivArea_t;
    public:
    Illumination(const std::vector<Stratum>& strata, int lightDistributionFactor, float diffusion);

    /**
     * \brief remove all registered biomass
     * \details remove all references to individuals. Usually called on all cells prior to distributeIndividuals()
     */
    void clearIndividuals();

    /**
     * \brief distributes individuals to this and neighbouring voxels
     * \details a maximum of stratum.size area is distributed to this voxel, the rest is passed on to neighbours
     */
    void distributeIndividuals (Community& community);

    /**
     * \brief direct light to plant individuals
     * \details light is distributed to the individuals living here, starting with the uppermost stratum.
     * light enters at 90 degree angle
     */
    void sendLightBeam(int light);

    /**
     * \brief add neighbouring voxels
     * \details neighbouring voxels may spill over light and plant material
     */
    void addNeighbours(const std::vector<Illumination*>& m_illumin);

    /**
     * \brief Get plant surface area
     * \details returns total plant surface of any material in this voxel (including spillover from neighbours).
     * Does not return material from recently died plants if includeDead is false. Reason: A timestep in the plant model 
     * first attributes light, including to neighbouring cells, then ages the plants. Calling getPlantCover after a time step
     * could cause the voxel to appear empty, even though it was not at the beginning of the time step.
     * @return float 
     */
    float getPlantCover(bool includeDead = true) const;


    private:
    const std::vector<Stratum> strata; /**<contains information about the stratification (number and size of each stratum, shading by buildings)*/
    int lightDistributionFactor; /**< determines how much the light is distributed among plants when TotalArea > voxelArea. 1 = single plant may take all */
    float diffusion; /**< the percentage of light that is diffused and bypasses the strata */
    std::map<Stratum, std::vector<IndivArea_t>> surfaceAreas; /**< list of individuals and the area they cover in each stratum*/
    std::vector<Illumination*> neighbours; //light calculation of neighbouring cells. Plants may spill over.

    /**
    * \brief choose which plants receive light
    * \details When there is too much material in the voxel (both from plants growing here and spilling over 
    * from neighbours), a top layer  of plant material is randomly selected to receive light. 
    * Light is distributed in batches, the number of batches is controlled by lightDistributionFactor.
    * \param stratum 
    */
    void choosePlants(const Stratum& stratum);

    /**
     * \brief distribute light resources to individuals in a stratum
     * \details Light resources are distributed to the individuals living here (see also distributeIndividuals()
     * The resources are distributed in proportion to the area they cover within the stratum. 
     * If the individual areas do not sum to stratum->Area, unspent resources are returned; 
     * if the sum is larger, winners are drawn randomly (see choosePlants). 
     * @return int unspent light resources
     */
    int distributeLightToIndividuals(int light, const Stratum& stratum);

    /**
     * \brief add individual area to this or neighbouring voxels
     * \details recursive function. Attempts to add to this voxel. If the area is too large,
     * calls itself on neighbouring voxels. the area is split into equal parts and distributed to the neighbours.
     * If neighbour was visited before, it is skipped.
     */
    void addHereOrElsewhere(IndivArea_t individual, const Stratum& stratum);
};
#endif //ILLUMINATION_H