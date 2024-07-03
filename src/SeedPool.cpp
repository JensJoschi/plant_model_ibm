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
 // Jens Joschinski (IBM)
 // ----------------------------------------------------------------------------

#include "SeedPool.h"
#ifdef min 
    #undef min
#endif
/** @cond */
#include <cmath>
/** @endcond */

SeedPool::SeedPool(const Traits* traits, const int c) : m_seedBiology_ptr(traits->seedBiol), 
    m_capacity(c), m_activeSeeds(0), m_dormantSeeds(0){
    assert(m_seedBiology_ptr);
    assert(m_capacity >= 0);
}

SeedPool::SeedPool(const SeedBiology* seedBiology, const int c) : m_seedBiology_ptr(seedBiology), 
    m_capacity(c), m_activeSeeds(0), m_dormantSeeds(0){
    assert(m_seedBiology_ptr);
    assert(m_capacity >= 0);
}

void SeedPool::increase(int n, bool active){
    assert(n>=0);
    assert(m_activeSeeds + m_dormantSeeds <= m_capacity);
    int remainingCapacity = m_capacity - m_activeSeeds - m_dormantSeeds;
    if (active){
        m_activeSeeds += std::min(n, remainingCapacity);
    } else{
        m_dormantSeeds += std::min(n, remainingCapacity);
    }
}

int SeedPool::age(int distMagnitude){//age(const std::map<string, float>& disturbance, bool onlyActive)
    activate();
    decrease(distMagnitude); //replace by disturb, which will call decrease() multiple times
    int germinated = germinate();
    killDormant();
    return (germinated);
}

//==============================================================================
int SeedPool::decrease(int n, bool onlyActive){
    assert(n>=0);
    //handle cases where n > seedPool:
    if (onlyActive && n > m_activeSeeds){
        int t = m_activeSeeds; 
        m_activeSeeds = 0;
        return (t);
    }
    if (!onlyActive && (n > m_activeSeeds + m_dormantSeeds)){
        int t = m_activeSeeds + m_dormantSeeds;
        m_activeSeeds = 0;
        m_dormantSeeds = 0;
        return (t);
    }
    //now n < pool; proportional reduction of active and dormant seeds
    int effectivePool = m_activeSeeds + m_dormantSeeds * !onlyActive;
    if (effectivePool == 0) return (0);
    float proportion = static_cast<float> (m_activeSeeds) / effectivePool;
    int activeSeedsToRemove = n * proportion; //potential truncation (rounding down)
    int dormantSeedsToRemove = n * (1 - proportion);

    int remainder = n - activeSeedsToRemove - dormantSeedsToRemove;
    if (remainder> 0){
        if (m_activeSeeds > activeSeedsToRemove){
            activeSeedsToRemove += remainder;
        } else if (m_dormantSeeds > dormantSeedsToRemove){
            dormantSeedsToRemove += remainder;
        } else {}//with both pools empty, we cannot remove any more seeds
    }
    m_activeSeeds -= activeSeedsToRemove;
    m_dormantSeeds -= dormantSeedsToRemove;
    assert(m_activeSeeds >= 0);
    assert(m_dormantSeeds >= 0);
    return (activeSeedsToRemove + dormantSeedsToRemove);
}

int SeedPool::germinate(){
    int germinated = std::round(m_seedBiology_ptr->germinationSuccess * m_activeSeeds);
    m_activeSeeds = 0;
    return (germinated);
}

void SeedPool::activate(){
    assert(m_seedBiology_ptr->dormancyBreakRate >= 0.0 && m_seedBiology_ptr->dormancyBreakRate <= 1.0);
    int activated = std::round(m_seedBiology_ptr->dormancyBreakRate * m_dormantSeeds);
    m_dormantSeeds -= activated;
    m_activeSeeds += activated;
}

void SeedPool::killDormant (){
    assert (m_seedBiology_ptr->mortalityDormant >= 0.0 && m_seedBiology_ptr->mortalityDormant <= 1.0);
    m_dormantSeeds -= std::round(m_seedBiology_ptr->mortalityDormant * m_dormantSeeds);
}

