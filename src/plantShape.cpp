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
 // --------------------------------------------------------------------------

#include "plantShape.h"

PlantShape::PlantShape(const nlohmann::json& traits) {
    try {
        m_density = traits.at("density");
        check();
    } catch (nlohmann::json::exception& e) {
        LOG(DEBUG) << "fields in plant shape file are: " << traits.dump(4);
        LOG(ERROR) << "Exception when reading from json PlantShape: " << e.what() << '\n';
        throw;
    } catch (std::invalid_argument& e) {
        LOG(ERROR) << "Invalid argument when initializing PlantShape: " << e.what() << '\n';
        throw;
    }
}

float PlantShape::getArea(float biomass) const{
    return(biomass / m_density);
}

float PlantShape::getArea(float biomass, float height, float from, float to) const {
    assert(false && "Calling shape base class. This function should be implemented in derived classes");
    return 0.0;
}

void PlantShape::check(){
    if (m_density <= 0){
        throw std::invalid_argument("Invalid values for PlantShape traits");
    }
}

PlantRectangle::PlantRectangle(const nlohmann::json& traits) : PlantShape(traits) {}

float PlantRectangle::getArea(float biomass, float height, float from, float to) const {
    assert(biomass >=0.0 && height >= 0.0);
    assert(from >= 0.0 && from <= to);
    assert(to <= height);
    if (height == 0.0 || from == to) return 0.0f;
    float proportion = (to - from) / height;
    return (PlantShape::getArea(biomass) * proportion);
}

