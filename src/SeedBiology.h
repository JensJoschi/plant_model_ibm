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
 * \file SeedBiology.h
 * \brief Seed pool traits definition
 * \details this class contains a list of parameters that define a plant's seed atttributes. 
 */

#ifndef SEEDBIOL_H
#define SEEDBIOL_H

/** @cond */
#include "nlohmann/json.hpp"
/** @endcond */

 /*!
 * \class SeedBiology
 * \brief Seed Biology definition
 * \details 
 * This object stores all the parameters characterizing the seed biology of a plant. Parameters concern dormancy, mortality etc.
 * \note
 * This class is on purpose inaccessible except by one specialized class (SeedPool). Makes it easier to maintain the code and to add new features.
 */
class SeedBiology{
    friend class SeedPool;
	public:

	/**
	 * \brief Construct a new SeedBiology object from json file
     * \details The json file must contain the following entries:
     * - "Dormancy": can a seed be dormant?
     * - "GerminationSuccess": germination rate
     * - "MortalityDormant": The mortality of the seed when dormant (lower than Germinationsuccess)
     * - "DormancyBreakRate": The rate at which dormancy is broken every year.
	 * \param SeedBiologyTraits json object with all seed traits (mortality, dormancy etc)
	 */
	SeedBiology(const nlohmann::json& SeedBiologyTraits);

    private:
    bool dormancy; 
    float germinationSuccess;
    float mortalityDormant;
    float dormancyBreakRate;
    /**
     * \brief check if the parameters are consistent
     * \details
     */
	void check();
};

#endif // SEEDBIOL_H