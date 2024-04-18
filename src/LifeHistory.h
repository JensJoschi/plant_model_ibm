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
 // Jens Joschinski (IBM);
 // ----------------------------------------------------------------------------


/*!
 * \file LifeHistory.h
 * \brief Life history definition
 * \details this class contains a list of parameters that define a plant's life History atttributes. 
 */

#ifndef LIFEHISTORY_H
#define LIFEHISTORY_H

/** @cond */
#include "nlohmann/json.hpp"
/** @endcond */



 /*!
 * \class LifeHistory
 * \brief Plant life History definition
 * \details 
 * This object stores all the parameters characterizing the life history a plant. Parameters concern life span, maturation time etc.
 * \note
 * This class is on purpose inaccessible except by one specialized class (PlantGrowth). Makes it easier to maintain the code and to add new features.
 */
class LifeHistory{
	friend class PlantGrowth;
	public:

	/**
	 * \brief Construct a new LifeHistory object from json file
	 * \details The json file must contain the following entries:
	 * - "MaturationTime": time until the plant is mature
	 * - "LifeSpan": how long the plant lives
	 * - "MaxHeight": maximum height the plant can reach
	 * \param lifeHistoryTraits json object with all life history traits (maturity, lifespan etc)
	 */
	LifeHistory(const nlohmann::json& traits);

	private: 
	int MatAge;  
	int LifeSpan;
	int HMax; 
    /**
     * \brief check if the parameters are consistent
     */
	void check();
};

#endif // LIFEHISTORY_H