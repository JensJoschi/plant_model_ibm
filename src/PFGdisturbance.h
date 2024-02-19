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
 // RFate team: original idea
 // Jens Joschinski: reimplementation
 // --------------------------------------------------------------------------

/*!
 * \file p_disturbance.h
 * \brief plant functional group response to perturbation(s)
 * \author Jens Joschinski
 * \date 2023/03/29
 
 */

#ifndef P_DISTURBANCE_H
#define P_DISTURBANCE_H

#include <string>
#include <map>
#include <nlohmann/json.hpp>
#include "dist.h"
#include "FGUtils.h"

/**
 * \brief class to hold a list of disturbances
 * \details 
 * This class defines a list of disturbances that can be applied to a plant functional group.
 * The magnitude of the disturbances is defined in the Dist class, and contains the following:
 * - SeedKill: proportion of individuals killed
 * - PropResprout: proportion of individuals resprouting
 * - PropKilled: proportion of individuals killed
 */
class PFGDisturbances{
    public:
    friend class FuncGroup;
    explicit PFGDisturbances(const nlohmann::json& pfg);
    explicit PFGDisturbances() = default;
    std::string name; /*!< name of the PFG */

    private: 
    std::map<LifeStage, std::map<std::string, const Dist>> distResponse; /*!< PFG response to disturbances */
	// for every life stage we have a range of named disturbances, e.g. mouse, elephant, fire.
	// the dist class determines the amount of biomass killed, the number of seeds destroyed, and potentially the amount that can resprout. 
	// not sure about the resprouting though, better leave at zero. 
};
#endif