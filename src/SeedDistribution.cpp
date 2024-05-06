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

#include "SeedDistribution.h"
#include "Landscape.h"
#include "rng.h"
/** @cond */
#include <string>
#include <vector>
#include <string_view>
#include <map>
/** @endcond */

SeedDistribution::SeedDistribution(const std::vector<std::string_view>& types, const std::vector<std::string>& keys) {
	for (const auto& type : types) {
		Landscape<int> l {keys};
		seeds.insert(std::pair<std::string_view, Landscape<int>>(type, l));
	}
}

SeedDistribution::~SeedDistribution() {
	seeds.clear();
}


void SeedDistribution::addSeeds(int number, std::string_view type) {
	Landscape<int>& current = seeds.at(type);
	std::vector<std::string> keys = current.getKeys(); //not very nice but works for now.
	for (auto cell : keys) {
		current.setValue(cell, current.at(cell) + number);
	}
	//better: change landscape so one can use:
	//for auto& cell : current {
		// cell.second += number;
}

void SeedDistribution::addSeeds(int number) {
    for (auto& t : seeds){
        addSeeds(number, t.first);
        }
    }

const std::map<std::string_view, int> SeedDistribution::getSeeds(const std::string& cell) const{
	std::map<std::string_view, int> result;
	for (const auto& t : seeds){
		result.insert(std::pair<std::string_view, int>(t.first, t.second.at(cell)));
	}
	return result;
}

void SeedDistribution::disperse() {
	std::map<std::string_view, Landscape<int>> newValues;
	auto oneMap = seeds.begin();
	std::uniform_int_distribution<> ls{ 0, static_cast<int>(oneMap->second.getTotncell() - 1)};
	std::vector<std::string> keys = oneMap->second.getKeys(); //same for all maps anyway
	for (auto& t : seeds){
		Landscape<int>& current = t.second;
		int totalSeeds = 0;
		for(auto const& cell: current){
			totalSeeds += cell.second;
		}

		int seedsPerBatch = totalSeeds > (5 * oneMap->second.getTotncell()) ? totalSeeds / (5 * oneMap->second.getTotncell()) : 1;
		int batches = totalSeeds / seedsPerBatch;
		int remainder = totalSeeds % seedsPerBatch;

		Landscape<int> temp(keys);
		for (int s = 0; s < batches; s++){
			const std::string& key = keys[ls(RNGs::mersenne)];
				temp.setValue(key, temp.at(key) + seedsPerBatch);
		}
		if (remainder > 0){
			const std::string& key = keys[ls(RNGs::mersenne)];
			temp.setValue(key, temp.at(key) + remainder);
		}
		newValues.insert(std::pair<std::string_view, Landscape<int>>(t.first, temp));
    }
	seeds = newValues;
}