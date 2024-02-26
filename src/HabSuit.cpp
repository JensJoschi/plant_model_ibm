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

 #include "HabSuit.h"
#include "SoilRequirements.h"
#include "Traits.h"
/** @cond */
#include <memory>
#include <easylogging++.h>
/** @endcond */

HabSuit::HabSuit(const SoilRequirements* const traits, std::shared_ptr<Soil> soil) : 
    m_soilTraits_ptr(traits), m_soil_ptr(soil){
    assert(m_soilTraits_ptr);
    assert(m_soil_ptr);
    auto it = m_soilTraits_ptr->acceptedSoils.find(m_soil_ptr->m_name);
    if (it == m_soilTraits_ptr->acceptedSoils.end()){
        LOG(ERROR) << "Soil " << m_soil_ptr->m_name << " not found in acceptedSoils";
        throw std::runtime_error("Soil not found in acceptedSoils");
    }
}


HabSuit::HabSuit(const Traits* const traits, std::shared_ptr<Soil> soil) : 
    m_soilTraits_ptr(traits->soilReqs), m_soil_ptr(soil){
    assert(m_soilTraits_ptr);
    assert(m_soil_ptr);
    auto it = m_soilTraits_ptr->acceptedSoils.find(m_soil_ptr->m_name);
    if (it == m_soilTraits_ptr->acceptedSoils.end()){
        LOG(ERROR) << "Soil " << m_soil_ptr->m_name << " not found in acceptedSoils";
        throw std::runtime_error("Soil not found in acceptedSoils");
    }
}

bool HabSuit::isSuitable() const {
    auto it = m_soilTraits_ptr->acceptedSoils.find(m_soil_ptr->m_name);
    assert (it != m_soilTraits_ptr->acceptedSoils.end());
    bool typeFits = it->second;
    bool depthFits = m_soil_ptr->m_depth >= m_soilTraits_ptr->minDepth;
    return (typeFits && depthFits);
}