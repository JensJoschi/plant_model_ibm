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
 // RFate team: original idea
 // JJ: complete reimplementation
 // ----------------------------------------------------------------------------

/*!
 * \file p_dist.h
 * \brief Disturbance definition
 * \details defines by how much a disturbance affects a plant functional group
 * The disturbance can kill individuals, or destroy their seeds. The plant may resprout, however.
 * \author Jens Joschinski
 * \date 2023/03/29
 
 */

#ifndef P_DIST_H
#define P_DIST_H


struct Dist{

    double seedKill; /*!< proportion of seeds killed */
    double propResprout; /*!< proportion of biomass allowed to resprout */
    double propKilled;  /*!< proportion of biomass removed */
};

#endif