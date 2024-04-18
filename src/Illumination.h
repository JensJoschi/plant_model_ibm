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
 * Part of the components that make up a community
 */

#ifndef ILLUMINATION_H
#define ILLUMINATION_H

#include "Individual.h"

/** @cond */
#include <vector>
#include <map>
/** @endcond */

struct Stratum{
    float from; /**< The lower bound of the stratum */
    float to;   /**< The upper bound of the stratum */
    float area; /**< The area of the stratum */
    float shading;  /**< Shading by buildings etc, range 0-1 where 1 == full shade, blocks 100% of incoming light */
};

/*!
 * \class Illumination
 * \brief Plant community in a cell
 * \details 
 * Contains all variables and functions to calculate the light that reaches individuals Plants.
 * In this basic version, the community is stratified; light beams that hit the community at an 
 * angle of 90 degrees first hit the uppermost strata and plants therein. Any light that is not
 * absorbed by the plants in the uppermost strata is passed on to the next strata, and so on.
 * The remaining light leaving a stratum is diffused and homogenized over the full area of the next stratum. This means that
 * the plant shades itself partially but not completely. Example:
 * If a plant covers the whole length of a stratum, it will receive all 100% of the light; if it has 2/3 of its area in the uppermost
 * and 1/3 in the next, it receives 66% of the light in the uppermost, and 33% of the *remaining* light (11%) in the next, so 77% in total. 
 * The lower part  is therefore shaded and less efficient; and 33%*66% = 22% of the light passes through without hitting the plant. 
 * Without the homogenization the uppermost part would receive 66%, but the lower part would be entirely shaded and receive nothing; 
 * and 33% of the light would be lost.  
 * This class associates with individuals and is responsible for feeding them with light.
 * \note some resemblance to RFate and the ECOLOPES PLANT MODEL, but in this version
 * - light is an integer and not a factor;
 * - plants on the same stratum do compete for light; 
 * - plants still do shade themselves, but as the upper and lower  parts share resources, they do not kill themselves. 
 * \note ECOLOPES PLANT MODEL allows for angular light input, this is not yet implemented. See issue #8
 * \note the plant community may grow larger than the illuminated area (i.e. they grow out of the voxel cell). Pieces growing outwards
 * are entirely ignored (do not cause shade on neighbours, do not receive light from neighbour cells). See issue #9
 * */
class Illumination{
    public:
        Illumination(std::multimap<std::string_view, std::unique_ptr<Individual>>& individuals, const std::vector<Stratum>& strata);
        /**
         * \brief direct light to plant individuals
         * \details light is distributed to the individuals in the community, starting with the uppermost stratum.
         * light enters at 90 degree angle, and any plant material growing outside the voxel cell is ignored.
         * \param light 
         */
        void sendLightBeam(int light);

    private:
        std::multimap<std::string_view, std::unique_ptr<Individual>>& individual_refs; /**< Reference to the individuals in the community */
        const std::vector<Stratum> strata; /**<contains information about the stratification (number and size of each stratum, shading by buildings)*/

        /**
         * \brief distribute light resources to individuals in a stratum
         * \details Light resources are distributed to all biomass that occuring in one stratum. 
         * The resources are distributed among all individuals living here, in proportion to the area they cover within the stratum. 
         * If the individual areas do not sum to stratum->Area, unspent resources are returned. 
         * If plants are larger than illumArea and strict == true, the function will throw an error.
         * \param light number of light resources
         * \param stratum stratum to distribute light to
         * \param strict throw error if plants are larger than stratum?
         * @return int unspent light resources
         */
        int distributeLightStratum(int light, const Stratum& stratum, bool strict = false);

};

#endif //ILLUMINATION_H