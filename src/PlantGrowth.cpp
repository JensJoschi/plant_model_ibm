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

#include "PlantGrowth.h"
#include "LifeHistory.h"
#include "Traits.h"
/** @cond */
#include <cmath>
/** @endcond */

PlantGrowth::PlantGrowth(const LifeHistory* const traits): m_lifehist_ptr(traits),           m_height(0.0), m_age(0){}
PlantGrowth::PlantGrowth(const Traits* const traits):      m_lifehist_ptr(traits->lifeHist), m_height(0.0), m_age(0){}


int PlantGrowth::getHeight() const{
    return m_height;
}

float PlantGrowth::getProportion(int from, int to) const{
    assert(from >= 0);
    assert(to >= from);
    if (to > m_height) to = m_height;
    if (m_height == 0) return 0.0f;
    return static_cast<float>(to - from) / m_height;
}

bool PlantGrowth::age(){
    if ((m_age+1) > m_lifehist_ptr->LifeSpan) return false;
    else{ 
        m_age ++;
        double x0 = m_lifehist_ptr->MatAge;
        double k = 1.0;
        m_height = static_cast<int>(std::round(m_lifehist_ptr->HMax / (1 + exp(-k * (m_age - x0)))));
        return true;
    }
}