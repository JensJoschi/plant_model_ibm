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
 
#include "Illumination.h"
#include "Individual.h"

/** @cond */
#include <vector>
#include <map>
#include <algorithm>
/** @endcond */

Illumination::Illumination(std::multimap<std::string_view, std::unique_ptr<Individual>>& individuals, const std::vector<Stratum>& strata) : 
            individual_refs(individuals), strata(strata) {}

void Illumination::sendLightBeam(int light){
    assert(light >= 0);
    for (int i = strata.size()-1; i >= 0; --i){
        light = distributeLightStratum(light, strata[i]);
    }
}

//--PRIVATE FUNCTIONS--//


int Illumination::distributeLightStratum(int light, const Stratum& stratum, bool strict){
    assert(light >= 0);
    assert(stratum.from >= 0.0f && stratum.to > stratum.from);
    assert(stratum.shading >= 0.0f && stratum.shading <= 1.0f);
    assert(stratum.area >= 0.0f);

    //shading by buildings etc:
    light = light * (1.0f-stratum.shading);
    if (light == 0){
        return 0;
    }
    //find out how much light misses the individuals, handle case where community grows out of voxel
    float totalArea = 0.0f;
    for (auto& i : individual_refs){
        totalArea += i.second->getArea(stratum.from, stratum.to);
    }
    if ( strict && totalArea >= stratum.area) {throw std::runtime_error("Illumination::send light Beam: total Area >= stratum Area");}
    float amountThatHitsPlants = std::min (light * (totalArea/stratum.area), static_cast<float> (light));
    assert(amountThatHitsPlants <= light);
    //actual attribution of light resources
    for (auto& i : individual_refs){
        float area = i.second->getArea(stratum.from, stratum.to);
        float share = totalArea != 0.0f ? area/totalArea : 0.0f;
        int resources = std::round (share * amountThatHitsPlants);
        i.second->feed(resources);
    }
    return light - amountThatHitsPlants;
}

