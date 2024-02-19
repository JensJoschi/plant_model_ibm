/*------------------------------------------------------------------------------
Copyright (C)  2022 - present  Studio Animal-Aided Design

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
 * \file p_Individual.cpp
 * \brief single plant individual
 * \details 
 * \author Jens Joschinski
 * \version 1.0
 */


#include "Individual.h"
#include "PFG.h"
#include "ResourceAlloc.h"
/** @cond */
#include <vector>
#include <string>
/** @endcond */



Individual::Individual(const PFG* pfg): m_pfg_ptr(pfg), m_age(0), m_biomass(pfg->L), 
                        m_identity(pfg->name), m_resprouting(pfg->L), 
                        m_resPool_ptr(new PlantResource(&(pfg->allocation))){
}

Individual::~Individual(){
    delete m_resPool_ptr;//make unique
}

void Individual::feed(const int light, bool soilIsSuitable){
    m_resPool_ptr->updateResource(light);
};

bool Individual::doesItDie() const{
    return (m_age > m_pfg_ptr->L || m_resPool_ptr->isResourceCritical());
}

int Individual::useResources(){   
    Allocations toDistribute = m_resPool_ptr->allocateResources(getTotalBiomass());
    m_biomass += toDistribute.biomass;
    m_age++;
    return toDistribute.seeds;
}

int Individual::getTotalBiomass() const{
    return m_biomass;
}