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
 * \file plantShape.h
 * \brief Plant shape
 */

#ifndef PLANTSHAPE_H
#define PLANTSHAPE_H

/** @cond */
#include "nlohmann/json.hpp"
#include <cassert>
#include "easylogging++.h"
/** @endcond */

 /*!
 * \class PlantShape
 * \brief Plant shape
 * \details this class defines a plant's shape and allows calculating its surface area. 
 * The most important function of PlantShape is the calculation of the plant surface area, 
 * either of the total plant, or (more useful) of certain height section. The surface area 
 * determines how much light a plant can intercept, and is thus used to calculate 
 * the light household of a voxel cell.
 * All shapes (rectangle, lollipop etc) inherit from this class.
 * The only trait common to all shapes is density; 
 * the density of the plant describes how compact the biomass is distributed. 
 * Like Specific Leaf Area (SLA), it is expressed in m2/kg. However, in contrast to SLA, 
 * the density in this model describes the **area per fresh aboveground biomass**. (not dry biomass)
 * Density in this model can be calculated from SLA, water content and woodiness 
 * (possibly just SLA / 1000 or a similar universal scaling factor)
 * \note Note: density is the inverse of SLA. The variable will eveentually be renamed 
 * to specificArea, and the calculations will be inverted. 
 * For now, the variable put into the model should be "density": {scaling factor * 1/SLA} 
 * 
 */
class PlantShape{
	public:
	/**
	 * \brief Construct a new PlantShape object from json file
     * \details The json file must contain the following entries:
     * - "density": plant density, see class description
	 * \param traits json object with all shape traits
	 */
	explicit PlantShape(const nlohmann::json& traits);

    /**
     * \brief Get the total surface area of the plant
     * \details The surface area is the product of biomass and density and hence does not depend on a 
     * particular shape (will not be overridden in derived classes).
     * \param biomass the total biomass of the plant - usually provided by resource model
     */
    float getArea(float biomass) const;

     /**
     * \brief Get the area of the plant that falls between two heights
     * \details The area falling into a particular height section depends on the plant's shape
     * and may require further parameters (e.g. angles for triangular shape). This function is thus 
     * a virtual function and should not be called (see overrides in derived classes).
     * \param biomass the total biomass of the plant - usually provided by resource model
     * \param height current height of the plant - usually provided by plant growth model
     * \param from the lower bound of the requested height interval
     * \param to the upper bound of the requested height interval
     * \return the surface area of the plant that falls between the two heights
     */
    virtual float getArea(float biomass, float height, float from, float to) const;

	private: 
    float m_density; //describes how compact the biomass is distributed
    /**
     * \brief check if the parameters are consistent
     */
	void check();
};

/*!
 * \class RectangleShape
 * \brief plant of rectangular shape
 * \details this class defines a plant's shape as a rectangle and allows calculating its area. 
 * The surface area is homogenous along all the plant's height. 
 */
class PlantRectangle: public PlantShape{
    public:
    PlantRectangle(const nlohmann::json& traits);
    using PlantShape::getArea; //when compiler finds overloaded function in derived, it wont search base.
    /**
     * \brief Get the area of a rectangle between height intervals
     * \details The calculation of specific leaf area is simply: 
     * (height section/total height) * biomass/density
     *  Example: A 10 m high tree has a biomass of 1000 kg, so there are 200 kg 
     * of plant material in the section between 4 and 6 m. With a density of 0.1m2/kg, the 200 kg cover a surface area of 20 m2.
     */
    float getArea(float biomass, float height, float from, float to) const override;
};

#endif // PLANTSHAPE_H