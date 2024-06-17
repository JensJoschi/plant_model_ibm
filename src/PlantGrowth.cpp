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
#include <nlohmann/json.hpp>
/** @endcond */

PlantGrowth::PlantGrowth(const LifeHistory* const traits): m_lifehist_ptr(traits),           m_height(1.0), m_age(0){}
PlantGrowth::PlantGrowth(const LifeHistory* const traits, nlohmann::json j): m_lifehist_ptr(traits){
    try{
        m_height = j.at("height");
        m_age = j.at("age");
    }catch(std::out_of_range& e){
        throw std::invalid_argument("PlantGrowth: json does not contain height or age");
    }
    catch(nlohmann::json::type_error& e){
        throw std::invalid_argument("PlantGrowth: age must be of type int, height must be of type float");
    }
    check();
}

void PlantGrowth::check() const{
    if (m_height < 0.0 || m_age < 0) throw std::invalid_argument("PlantGrowth: height and age must be positive: " + std::to_string(m_height) + ", " + std::to_string(m_age));
    if (m_age > m_lifehist_ptr->LifeSpan) throw std::invalid_argument("PlantGrowth: age must not exceed lifespan: " + std::to_string(m_lifehist_ptr->LifeSpan));
    if (m_height > m_lifehist_ptr->HMax) throw std::invalid_argument("PlantGrowth: height must not exceed maximum height: " + std::to_string(m_lifehist_ptr->HMax));
}

float PlantGrowth::getHeight() const{
    return m_height;
}
int PlantGrowth::getAge() const{
    return m_age;
}

bool PlantGrowth::grow(){
    if ((m_age+1) > m_lifehist_ptr->LifeSpan) return false;
    else{ 
        m_age ++;
        double x0 = m_lifehist_ptr->MatAge;
        double k = 1.0;
        float newHeight = m_lifehist_ptr->HMax / (1 + exp(-k * (m_age - x0)));
        m_height = std::max (m_height, newHeight); //newly germinated plants may be larger than predicted by growth function
        return true;
    }
}