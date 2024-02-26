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
 * \file plantGrowth.h
 * \brief plant growth and aging
 * \details 
 */

#ifndef PLANTGROWTH_H
#define PLANTGROWTH_H

#include "LifeHistory.h"
#include "Traits.h"
/** @cond */

/** @endcond */

/**
 * \brief Plant Growth
 * \details
 * This classs lets the plant age, mature and grow in height. It also contains the general shape of the plant. The traits
 * defining the plant's growth are stored in the LifeHistory class.
 * PlantResource, PlantGrowth, EnvEffects and Habsuit together make up an Individual.
 * Plant shape is currently simplified to a rectangular shape, and biomass is homogenously distributed from 0 to height. 
 * Annual height growth affects the rectangle's height, and annual investment in biomass (resource-dependent) affects its width. 
 * A wider rectangle (more biomass per m height) consumes more light. The resource-dependent biomass growth is implemented in the PlantResource class.
 * For now, the plant's growth in height is resource-independent and uses a logistic growth function, 
 * with the inflection point being the age of maturity (when plants invest into seeds, their growth decelerates); the slope is arbitrarily set to 1.0.
 * \note In RFate, a slightly different growth formula was used, as the plants reached Immsize*MaxStratum at maturity/2. The curve apparently is not sigmoidal, 
 * but in the beginning a straight line, which then decelearates (no slow start). Immsize is hard to guess from data anyway so it was not used here.
 * \note this class has a rectangular base shape. One may derive a class with a different shape (lollipop, triangular) and override the 
 * getProportion(from, to) function
 */
class PlantGrowth{
    public:
    explicit PlantGrowth(const LifeHistory* const traits);
    explicit PlantGrowth(const Traits* const traits);

    int getHeight() const;

    /**
     * \brief Get the proportion of the plant's total area between two heights
     * \details
     * The plant has a rectangular shape. The proportion of the plants total area that falls 
     * between two heights x and y is (y-x)/height.
     * \param from the lower bound of the height interval
     * \param to the upper bound of the height interval
     * \return the proportion of the plants total area that falls between the two heights
     */
    float getProportion(int from, int to) const;

    /**
     * \brief let plant age and grow
     * \details growth is independent of resource availability. 
     * Growth is also indpendent of shading (no phenotypic plasticity).
     * It follows a logistic growth function with an inflection point at maturity
     * \return bool true if the plant is still alive, false if it has reached its lifespan
     */
    bool age();

    private:
    int m_age;
    int m_height;
    const LifeHistory* const m_lifehist_ptr;
};
#endif //PLANTGROWTH_H