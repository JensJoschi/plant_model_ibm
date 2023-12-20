/*------------------------------------------------------------------------------
Copyright (C) 2023 - present Jens Joschinski

This file is part of the ECOLOPES PLANT MODEL.

ECOLOPES PLANT MODEL is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your option) any later version.

ECOLOPES PLANT MODEL is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with ECOLOPES PLANT MODEL. 
If not, see <https://www.gnu.org/licenses/>. */

// --------------------------------------------------------------------------
 /* ECOLOPES PLANT MODEL is derived, modified and extended from FATE, https://github.com/leca-dev/RFate.git/ 
 * Copyright (C) 2021 Isabelle Boulangeat, Damien Georges, Maya Guéguen, Wilfried Thuiller 
 * For contributions to this particular file, see section "Authors and contributors".*/
// --------------------------------------------------------------------------

 // ----------------------------------------------------------------------------
 // Authors and contributors to this file:
 // Jens Joschinski
 // ----------------------------------------------------------------------------

 // ----------------------------------------------------------------------------
//THIS FILE IS FOR A NEW IMPLEMENTATION OF PLANT MODEL AND NOT BEING USED YET.
 // ----------------------------------------------------------------------------
 
/*!
 * \file p_Individual.h
 * \brief single plant individual
 * \details 
 * \author Jens Joschinski
 * \version 1.0
 */

#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include "p_PFG.h"
#include "p_PlantResource.h"
/** @cond */
#include <vector>
#include <string>
/** @endcond */

class Individual{
    public:
    Individual(const PFG* pfg);
    ~Individual();

    void feed(const int light, bool soilIsSuitable);
    void disturb(std::vector<int> amounts);
    int useResources();
    bool doesItDie() const;

    int getTotalBiomass() const;
    std::vector<int> getBiomassPerAge() const;
    std::vector<int> stratumToAges(int stratum) const;


    private:
    const PFG* const m_pfg_ptr;
    PlantResource* const m_resPool_ptr;
    int m_age;
    int m_biomass; //should biomass have an age distribution? Or single int?
    std::string m_identity;
    std::vector<int> m_resprouting;
};

#endif //INDIVIDUAL_H

