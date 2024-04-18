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


#include "SoilRequirements.h"
#include "Traits.h"
#include "Soil.h"
/** @cond */
#include <memory>
/** @endcond */

/**
 * \brief Habitat suitability
 * \details This class determines if a plant can grow in a specific habitat. 
 * The habitat is mostly determined by soil attributes. Biologically speaking, 
 * soil is a shared resource, i.e., multiple plants have access and may modify/deplete the soil. 
 * Suitability is a boolean attribute (a habitat is either suitable or not) and currently only defined by
 * soil type and depth. The soil type and depth are defined in the SoilRequirements class.
 * PlantResource, PlantGrowth, EnvEffects and Habsuit together make up an Individual.
 * \note even though soil is biologically a shared resource, it is computationally a weak_ptr,
 * because the Habitat Suitability class should not be responsible for the soil's lifetime.
 * It may access and modify the soil attributes (shared biological resource), but not control the 
 * memory allocation or deletion of soil (computational resource).
 */
class HabSuit{
    public:
    explicit HabSuit(const SoilRequirements* const traits, std::weak_ptr<Soil> soil);
    ~HabSuit();

    /**
     * \brief checks if current soil is still suitable
     * \details a soil is suitable if it exceeds the minimum required depth 
     * and if it is of a type that is accepted by the plant. To check whether a soil
     * would in principle be suitable (including space left), use wouldBeSuitable.
     */
    bool isCurrentlySuitable() const;

    /**
     * \brief check if a soil would be suitable for a plant
     * \details a soil is suitable if it exceeds the minimum required depth,
     * is of a type that is accepted by the plant, and if there is enough space left.
     * This static version should be used prior to creating an individual.
     * For an indidual already living on a (changing) soil, use isCurrentlySuitable.
     * \param traits 
     * \param soil 
     * @return true 
     * @return false 
     */
    static bool wouldBeSuitable(const SoilRequirements* const soilReqs, std::weak_ptr<Soil> soil){
        auto temporary = soil.lock();
        auto it = soilReqs->acceptedSoils.find(temporary->m_name);
        if (it == soilReqs->acceptedSoils.end()) throw std::runtime_error("Soil not found in acceptedSoils");
        assert (it != soilReqs->acceptedSoils.end());
        bool typeFits = it->second;
        bool depthFits = temporary->m_depth >=  soilReqs->minDepth;
        bool sizeFits = temporary->m_space >= soilReqs->size;
        return (typeFits && depthFits && sizeFits); //temporary goes out of scope, resetting the shared_ptr.use_count
    }

    private:
    std::weak_ptr<Soil> const m_soil_ptr;
    const SoilRequirements* const m_soilTraits_ptr;
};
#endif //HABSUIT_H