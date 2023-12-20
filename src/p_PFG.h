/*------------------------------------------------------------------------------
Copyright (C) 2023 - present Jens Joschinski

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

 // --------------------------------------------------------------------------
 // Authors and contributors to this file:
 // RFate team
 // JJ: partial reimplementation, addition of some new variables
 // --------------------------------------------------------------------------

/*!
 * \file p_PFG.h
 * \brief Plant Functional Group definition
 * \details this class contains a list of parameters that define a plant functional
 * group (e.g. longevity, height). A plant functional group (PFG) is a group of species
 * with similar traits. The PFG is the basic unit of the plant model.
 * This class is usually only constructed through PFGDefs, which creates all PFGs of the 
 * model at once.
 * \author Damien Georges, Maya Gueguen, Jens Joschinski
 * \version 2.0
 * \date 2023
 */

#ifndef PFG_H
#define PFG_H

#include "g_FGUtils.h"
#include "nlohmann/json.hpp"
#include "p_ResourceAlloc.h"

/** @cond */
#include <vector>
#include <string>
#include <string_view>
/** @endcond */

 /*!
 * \class PFG
 * \brief Plant Functional Group definition
 * \details 
 * This object stores all the parameters characterizing a Plant Functional
 * Group. Parameters concern life history and propagule biology, 
 * light and soil requirements * 
 * \note
 * Data is intentionally kept private, only accessible by the friend class FuncGroup.
 * Parameters are difficult to interpret, and FuncGroup will require some cleanup. 
 * Hiding the PFG parameters ensures encapsulation of the problem (FuncGroup' public functions are fine)
 */
class PFG{
	public:
	friend class FuncGroup;
	friend class Individual; //testing only, remove

	/**
	 * \brief Construct a new PFG object from a json file
	 * \param pfg PFG parameters
	 */
	PFG(const nlohmann::json& pfg);

	/*!
	 * \brief Full constructor
	 used to create default definitions in PFGDefs().
	*/
	PFG(std::string_view name, const int m, const int l, const Abund& maxa, const double& immsize, const int maxstratum,
	const std::vector<int>& strata, const std::vector<int>& pooll, const int potentialfecundity, const int lightshadefactor,
	const std::vector<Fract>& lightactivegerm, const std::vector<std::vector<bool>>& lighttolerance,  const std::vector<std::string>& soiltol, const int DepthReq);


	std::string name;
//==============================================================================
	private: 
	int M;                  /*!< Maturation time */
	int L;                  /*!< Life span */
	Abund MaxA;             /*!< Maximum Abundance [none, low, medium, high] */
	double ImmSize;         /*!< Proportion of immature plants relative to mature abundance */
	int MaxStratum;         /*!< Maximum stratum reached */
	std::vector<int> Strata;/*!< age at which PFG changes stratum */

	/* Propagule biology */
	std::vector<int> PoolL; /*!< Seed Pool Life Spans; for active and for dormant pool  */
	// bool m_InnateDorm;   /*!< Do FG seeds have Innate dormancy properties */
	int potentialFecundity; /*!< Potential Fecundity of mature plants */

	/* Light interaction module */
	int LightShadeFactor;               /*!< how much shade does one biomass unit produce */
	std::vector<Fract> LightActiveGerm; /*!< Proportion of Active seeds able to germinate under each light conditon. Vector of size 3[low, middle, high] */
	std::vector< std::vector<bool> > LightTolerance; /*!<can biomass of each growth stage survive unde reach light condition? 4 vectors (1 for each growth stage) of size 3 (low, middle, high) */

	/*soil module */
	std::vector<std::string> soilTol;    /*!< list of soil classes on which the PFG can live */
	int DepthReq;                        /*!< minimum soil depth that allows the PFG to live here */


	void check(); //used during construction to check that all parameters are set correctly

	ResourceAlloc allocation; /*!< Describes how light is converted to resources, and how resources are allocated to growth and reproduction*/
};

#endif // PFG_H
