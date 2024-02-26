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

/*!
 * \file Habsuit.h
 * \brief Habitat suitability
 * \details 
 */

#ifndef HABSUIT_H
#define HABSUIT_H


#include <string>
//to move in sep file
class Soil{
    public:
    Soil(int capacity, int depth, const std::string& name): m_capacity(capacity), m_depth(depth), m_name(name){};
    void updateSoilClass(const std::string& name){m_name = name;};
    const int m_capacity;
    const int m_depth;
    std::string m_name;
};



#include "SoilRequirements.h"
#include "Traits.h"
/** @cond */
#include <memory>
/** @endcond */

/**
 * \brief Habitat suitability
 * \details This class determines if a plant can grow in a specific habitat. 
 * The habitat is mostly determined by soil attributes. Soil is a shared resource,
 * i.e., multiple plants have access and may modify/deplete the soil. Suitability is a 
 * boolean attribute (a habitat is either suitable or not) and currently only defined by
 * soil type and depth. The soil type and depth are defined in the SoilRequirements class.
 * PlantResource, PlantGrowth, EnvEffects and Habsuit together make up an Individual.
 */
class HabSuit{
    public:
    explicit HabSuit(const SoilRequirements* const traits, std::shared_ptr<Soil> soil);
    explicit HabSuit(const Traits* const traits, std::shared_ptr<Soil> soil);

    bool isSuitable() const;

    private:
    std::shared_ptr<Soil> const m_soil_ptr;
    const SoilRequirements* const m_soilTraits_ptr;
};
#endif //HABSUIT_H