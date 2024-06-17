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



/*!
 * \file SeedDistribution.h
 * \brief Landscape-scale distribution of seed masses
 * \details contains the seeds that are currently "in the air" and not yet settled in individual voxel cells. 
 * This includes seeds that were produced in each cell, but also thos flying in from the region.
 */

#ifndef SEEDDISTRIBUTION_H
#define SEEDDISTRIBUTION_H

#include "Landscape.h"
/** @cond */
#include <string>
#include <vector>
#include <string_view>
#include <map>
/** @endcond */


class SeedDistribution{
    public:
    SeedDistribution(const std::vector<std::string_view>& types, const std::vector<Coordinates>& keys);
    SeedDistribution() = default;
    ~SeedDistribution();
    void addSeeds(int number);
    void addSeeds(int number, std::string_view type);
    const std::map<std::string_view, int> getSeeds(const Coordinates& cell) const;
    void disperse();

    private:
    std::map<std::string_view, Landscape<int>> seeds;


};

#endif // SEEDDISTRIBUTION_H