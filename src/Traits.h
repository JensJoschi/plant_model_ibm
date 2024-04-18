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
 * \file Traits.h
 * \brief Plant trait definition
 * \details this class contains a list of parameters that define a plant. 
 */

#ifndef TRAITS_H
#define TRAITS_H

#include "ResourceAlloc.h"
#include "LifeHistory.h"
#include "SeedBiology.h"
#include "SoilRequirements.h"
#include "plantShape.h"

/** @cond */
#include <vector>
#include <string>
#include "nlohmann/json.hpp"
/** @endcond */

 /*!
 * \class Traits
 * \brief Plant definition
 * \details 
 * This object stores all the parameters characterizing a plant. Parameters concern life history, allocation of resources,
 * seed biology and soil requirements, as well as their shape  
 * \note
 * The five pointers are public, but the classes they point to are on purpose inaccessible except by one specialized class each 
 * (e.g. ResourceAlloc is only available for PlantResource). Future (better) PlantResource versions thus do not need to rewrite Traits but only ResourceAlloc.
 */
class Traits{
	public:

	/**
	 * \brief Construct a new Traits object from json files
	 * \param lifeHistoryTraits json object with all life history traits (maturity, lifespan etc)
	 * \param allocationTraits json object with all resource allocation traits (light conversion, allocation to growth and reproduction)
	 * \param soilTraits json object with all soil traits (tolerance, depth requirements)
	 * \param seedTraits json object with all seed traits (seed pool life spans, dormancy)
	 * \param shapeTraits json object with all shape traits (currently only density)
	 */
	Traits(const nlohmann::json& lifeHistoryTraits, const nlohmann::json& allocationTraits, 
		const nlohmann::json& soilTraits, const nlohmann::json& seedTraits, const nlohmann::json& shapeTraits);
/**
	 * \brief Construct a new Traits object from json file
	 * \param traits json object with all traits
	 */
	Traits(const nlohmann::json& traits);

	const ResourceAlloc* const allocation;
	const LifeHistory* const lifeHist;
	const SeedBiology* const seedBiol;
	const SoilRequirements* const soilReqs;
	const PlantRectangle* const shape;
};

#endif // PFG_H
