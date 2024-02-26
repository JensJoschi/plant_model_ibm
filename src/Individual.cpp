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

#include "Individual.h"
#include "Traits.h"
#include "PlantGrowth.h"
#include "PlantResource.h"
#include "HabSuit.h"

/** @cond */
#include <iostream>
#include <memory>
#include "easylogging++.h"
#include <optional>
#include <utility>
#include <map>
#include <cassert>
#include <cmath>
/** @endcond */

Individual::Individual(const Traits* traits, std::shared_ptr<Soil> soil): 
    m_growth_ptr    (std::make_unique<PlantGrowth>  (traits->lifeHist)), 
    m_habSuit_ptr   (std::make_unique<HabSuit>      (traits->soilReqs, soil)), 
    m_resource_ptr  (std::make_unique<PlantResource>(traits->allocation)){
    assert(m_growth_ptr);
    assert(m_habSuit_ptr);
    assert(m_resource_ptr);
}

void Individual::feed(const int light){
    assert(light >=0);
    m_resource_ptr->updateResource(light, m_habSuit_ptr->isSuitable());
};

std::map<std::string, float> Individual::disturb(const std::map<std::string, float>& disturbance){
    LOG(FATAL) << "Disturbance not implemented yet.";
    return std::map<std::string, float>();
}

std::pair<bool, int> Individual::age(){
    bool diesOfAge = !m_growth_ptr->age();
    if (diesOfAge){
        //add disturbance on dead plant (biomass can still be eaten)
        return std::make_pair(false, 0);
    }
    else{
        //add disturbance
        return m_resource_ptr->allocateResources();
    }
}

int Individual::getBiomass(int from, int to, bool shadingCorrected) const{
    assert(from >= 0);
    assert(to >= from);
    return std::round(m_resource_ptr->getBiomass(shadingCorrected) * m_growth_ptr->getProportion(from, to));
}