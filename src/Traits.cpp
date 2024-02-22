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
 // Jens Joschinski (IBM); rewrite of PFG class (RFATE/EPM)
 // --------------------------------------------------------------------------


#include "Traits.h"
#include "ResourceAlloc.h"
#include "LifeHistory.h"
#include "SeedBiology.h"
#include "SoilRequirements.h"

/** @cond */
#include "nlohmann/json.hpp"
#include "easylogging++.h"
/** @endcond */

Traits::Traits(const nlohmann::json& lifeHistoryTraits, const nlohmann::json& allocationTraits, const nlohmann::json& soilTraits, const nlohmann::json& seedTraits)
    : allocation(new ResourceAlloc(allocationTraits)), 
      lifeHist(new LifeHistory(lifeHistoryTraits)), 
      seedBiol(new SeedBiology(seedTraits)), 
      soilReqs(new SoilRequirements(soilTraits)) {
}

Traits::Traits(const nlohmann::json& traits): allocation(new ResourceAlloc(traits)), 
                                              lifeHist(new LifeHistory(traits)),
                                              seedBiol(new SeedBiology(traits)), 
                                              soilReqs(new SoilRequirements(traits)) {
}
