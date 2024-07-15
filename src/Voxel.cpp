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
#include "Voxel.h"
#include "Community.h"
#include "Illumination.h"
#include "Soil.h"
#include "Landscape.h"

/** @cond */
#include <memory>
#include <map>
#include <string>
#include <vector>
#include <string_view>
#include <nlohmann/json.hpp>
/** @endcond */



Voxel::Voxel(int capacity, int depth, const std::string& name, 
    std::map<std::string_view, const Traits*> traits, int maxIndividuals, int initialSeeds,
    const std::vector<Stratum>& voxel, int lightDistributionFactor, float diffusion):
    m_illumination(voxel, lightDistributionFactor, diffusion),
    m_soil(std::make_shared<Soil>(capacity, depth, name)),
    m_community(m_soil, traits, maxIndividuals, initialSeeds){}

Voxel::~Voxel(){}

// Voxel::Voxel(int capacity, int depth, const std::string& name, 
//     std::map<std::string_view, const Traits*> traits, int maxIndividuals, int initialSeeds,
//     const std::vector<Stratum>& voxel,int lightDistributionFactor,float diffusion,
//     const nlohmann::json& individuals):
//     m_illumination(voxel, lightDistributionFactor, diffusion),
//     m_soil(std::make_shared<Soil>(capacity, depth, name)),
//     m_community(m_soil, traits, maxIndividuals, initialSeeds, individuals){}

void Voxel::clearArea(){
    m_illumination.clearIndividuals();

}
void Voxel::distributeArea(){
    m_illumination.distributeIndividuals (m_community);
    }

void Voxel::sendLightBeam(int light){
    m_illumination.sendLightBeam(light);
}
void Voxel::addNeighbours(const std::vector<Voxel*>& neighbours){
    std::vector<Illumination*> Illuminations;
    for (auto& neighbour : neighbours){
        Illuminations.push_back(&neighbour->m_illumination);
    }
    m_illumination.addNeighbours(Illuminations);
}

void Voxel::rainSeeds(const std::map<std::string_view, int>& seeds){
    m_community.rainSeeds(seeds);
}
std::map<std::string, int> Voxel::age(){
    return m_community.age();
}
int Voxel::getCount(std::string_view type) const {
    return m_community.getCount(type);
}
int Voxel::getCount (int from, int to, std::string_view type ) const{
    return m_community.getCount(from, to, type);
}
float Voxel::getBiomass(std::string_view type ) const {
    return m_community.getBiomass(type);
}

float Voxel::getPlantCover() const{
    return m_illumination.getPlantCover();
}