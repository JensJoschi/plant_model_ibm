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
 // RFate team: provide original code, SimulMap (RFATE)
 // JJ: split SimulMap into a PlantModel class and a EcolopesLand class (ECOLOPES JOINT MODEL); 
 //     rewrote habitat suitability and disturbance functions (EPM)
 //     functions to exchange data with other models (ECOLOPES JOINT MODEL)
 // VMVC: functions to exchange data with other models (ECOLOPES JOINT MODEL)
  // JJ: rewrite (IBM)
 // ----------------------------------------------------------------------------

#include "PlantModel.h"
#include "Landscape.h"
#include "GSP_PLANTS.h"
#include "Data_PLANTS.h"
#include "rng.h"
#include "config.h"
#include "Voxel.h"

/** @cond */
#include <vector>
#include <string>
#include <map>
#include "easylogging++.h"
/** @endcond */

#ifndef DEFAULT_WD
std::string def_WD = "";
#else
std::string def_WD= DEFAULT_WD;
#endif

//==============================================================================
//constructor

PlantModel::PlantModel(	const nlohmann::json& globalParams, 
			const nlohmann::json& voxelData, 
			const nlohmann::json& TraitDefinitions, 
			const std::string& logConfig, bool FixRNG): 
			m_settings(globalParams), m_voxelData(voxelData, m_settings), 
			m_traits(fillTraits(TraitDefinitions)),
			m_cells(m_voxelData.keyList){
	assert(m_settings.noStrata > 0);
	assert(m_traits.size() > 0);
	el::Configurations conf;
	conf = logConfig.empty()? el::Configurations(m_voxelData.logger) : el::Configurations(logConfig);
	el::Loggers::reconfigureAllLoggers(conf); 
	LOG(INFO) << "Model version: " << VERSION;
	std::string WD = def_WD;
	if (!m_voxelData.checkKeys(m_settings)){LOG(FATAL) << "keys of inputs do not match";}
	if (FixRNG){
		LOG(INFO) << "fix RNG with seed 2230";
		RNGs::mersenne = std::mt19937{2230};
	}

	//-----------------------------------------------------------------------------
//to do: 
//check that each data.allowedTypes entry is also in traits
//remove those traits that do not occur anywhere in data.allowedTypes
	//-----------------------------------------------------------------------------

	LOG(INFO) << "Create data structures";

	if(m_settings.seedRain>0 || m_settings.doesDispersal){
		std::vector<std::string_view> types;
		for (auto it = m_traits.begin(); it != m_traits.end(); ++it){
			types.push_back(it->first);
		}
		m_seeds = SeedDistribution(types, m_voxelData.keyList);
	} //else: keep empty (not zero). Empty maps ensure assertion failures if used.

	//Voxel object
	for (const auto& coord : m_voxelData.keyList){  
		int capacity = m_settings.soilCapacity;
		int depth = m_voxelData.soilDepth.at(coord);
		const std::string& soilName = m_voxelData.soilClass.at(coord);
		int maxIndividuals = m_settings.startingPopSize;
		int initialSeeds = m_settings.initialSeeds;
		int lightDistributionFactor = m_settings.lightDistributionFactor; 
		float diffusion = m_settings.diffusion;

		std::map<std::string_view, const Traits*> traits;
		if (m_settings.doesRegionalModel){
			const std::vector<std::string>& availableTypes = m_voxelData.allowedTypes.at(coord);
			for (auto type = availableTypes.begin(); type != availableTypes.end(); ++type){
				traits.emplace(*type, &m_traits.at(*type));
			} 
		}else {
			for (auto it = m_traits.begin(); it != m_traits.end(); ++it){
				traits.emplace(it->first, &it->second);
			}
		}

		std::vector<Stratum> voxel;
		float sizePerStratum = m_settings.voxelHeight/m_settings.noStrata;
		for (int i = 0; i < m_settings.noStrata; i++){
    		float from = i * sizePerStratum;
    		float to = from + sizePerStratum;
			voxel.emplace_back(Stratum{from, to, m_settings.voxelArea });
		}

		m_cells.setValue(coord, new Voxel(capacity, depth, soilName, traits, maxIndividuals, 
						initialSeeds, voxel, lightDistributionFactor, diffusion));
	}
	for (const auto& coord : m_voxelData.keyList){  
			m_cells.at(coord)->addNeighbours(m_cells.getNeighbourValues2D(coord));
	}
	// to do:
	// 	int depth = m_plantInputs_ptr->config.doesSoilDepth ? m_plantInputs_ptr->data.soilDepth.at(*it) : 0;
}

std::map<std::string, Traits> PlantModel::fillTraits (const nlohmann::json& TraitDefinitions){
	std::map<std::string, Traits> traits;
	for (auto it = TraitDefinitions.begin(); it != TraitDefinitions.end(); ++it){
		traits.emplace(it.key(), Traits(it.value()));
	}
	return traits;


// 	for (auto& [key, value] : j.items()){
// 		try{ 
//       traitlist.emplace(key, Traits(value));
//     }
// 		catch (const std::runtime_error& e){
// 			LOG(WARNING) << e.what();
// 		 	LOG(FATAL) << "error in trait definition" << ". Check your json file.";
// 		}
// 	}
//   std::map<std::string_view, const Traits*> traits;
//   for (auto& [key, value] : traitlist){
//     traits.insert(std::pair<std::string_view, const Traits*>(key, &value));
//   }
}

//==============================================================================
//public

void PlantModel::TPlusOne(){
	if (m_settings.seedRain > 0) m_seeds.addSeeds(m_settings.seedRain);

	LOG(DEBUG) << "--Succession";
	for (auto cell:m_cells){cell.second->clearArea();}

	for (auto cell: m_cells){//this seems like an ideal place for parallelization.
		cell.second->rainSeeds(m_seeds.getSeeds(cell.first));
		cell.second->distributeArea();
		cell.second->sendLightBeam(m_voxelData.light.at(cell.first));
	}
	//light calculations are affected by indviduals of neighbour cells (spillover of biomass). 
	//killing individuals while looping through cells causes headaches with expired ptrs, so better do aging in seperate loop.
	for (auto cell:m_cells){
		std::map<std::string, int> newSeeds = cell.second->age();
	if ( m_settings.doesDispersal) {
		for (auto it = newSeeds.begin(); it != newSeeds.end(); ++it){
			m_seeds.addSeeds(it->second, it->first);
		}
	}
	}

	if ( m_settings.doesDispersal) {
		LOG(DEBUG) << "--Dispersal";
		m_seeds.disperse();
	}
}

//--------------------------------------------------------------------------

Landscape<int> PlantModel::getNumber(const std::string& type) const{
	Landscape<int> out(m_voxelData.keyList);
	for (auto cell: m_cells){
		out.setValue(cell.first, cell.second->getCount(type));
	}
	return out;
}

Landscape<std::map<std::string, int>> PlantModel::getNumber() const{
	Landscape<std::map<std::string, int>> out(m_voxelData.keyList);
	for (auto cell: m_cells){
		std::map<std::string, int> contentOfACell;
		for (auto it = m_traits.begin(); it != m_traits.end(); ++it){
			contentOfACell.emplace(it->first, cell.second->getCount(it->first));
		}
		out.setValue(cell.first, contentOfACell);
	}
	return out;
}
Landscape<float> PlantModel::getArea() const{
	Landscape<float> out(m_voxelData.keyList);
	for (auto cell: m_cells){
		out.setValue(cell.first, cell.second->getPlantCover());
	}
	return out;
}
Landscape<std::map<std::string, int>> PlantModel::getNumber(int from, int to) const{
	Landscape<std::map<std::string, int>> out(m_voxelData.keyList);
	for (auto cell: m_cells){
		std::map<std::string, int> contentOfACell;
		for (auto it = m_traits.begin(); it != m_traits.end(); ++it){
			contentOfACell.emplace(it->first, cell.second->getCount( from,  to, it->first));
		}
		out.setValue(cell.first, contentOfACell);
	}
	return out;
}

Landscape<std::map<std::string, float>> PlantModel::getBiomass() const{
	Landscape<std::map<std::string, float>> out(m_voxelData.keyList);
	for (auto cell: m_cells){
		std::map<std::string, float> contentOfACell;
		for (auto it = m_traits.begin(); it != m_traits.end(); ++it){
			contentOfACell.emplace(it->first, cell.second->getBiomass(it->first));
		}
		out.setValue(cell.first, contentOfACell);
	}
	return out;
}


void PlantModel::saveArea(int t, const std::string& fileName) const {
	auto& saveYears = m_settings.m_saveYears;
	if (std::find(saveYears.begin(), saveYears.end(), t) != saveYears.end()){
		std::string prefix = fileName.empty() ? "" : fileName + "_";
		std::string fileName = prefix + "area_" + std::to_string(t) + ".json";
		Landscape<float> area = getArea();
		nlohmann::ordered_json orderedArea;
		for (auto cell: m_voxelData.keyList){
			orderedArea[cell.to_string()] = area.at(cell);
		}
		std::ofstream fileArea(m_voxelData.savingDir + fileName);
		fileArea << orderedArea.dump(4);
		fileArea.close();
	}
	else {LOG(DEBUG) << "not saving";}
}
void PlantModel::save(int t, const std::string& fileName, bool sum, bool ordered) const{
	auto& saveYears = m_settings.m_saveYears;
	if (std::find(saveYears.begin(), saveYears.end(), t) != saveYears.end()){
		std::string prefix = fileName.empty() ? "" : fileName + "_";
		std::string fileNameBM = prefix + "biomass_" + std::to_string(t) + ".json";
		std::string fileNameAB = prefix + "abundance_" + std::to_string(t) + ".json";
		std::string fileNameSumBM = prefix + "summed_biomass_" + std::to_string(t) + ".json";
		std::string fileNameSumAB = prefix + "summed_abundance_" + std::to_string(t) + ".json";

		LOG(INFO) << "SAVING";
		Landscape<std::map<std::string, int>> abundance = getNumber(2, m_settings.voxelHeight);
		Landscape<std::map<std::string, float>> biomass = getBiomass();
		Landscape<int> summedAbundance{m_voxelData.keyList};
		Landscape<float> summedBiomass{m_voxelData.keyList};
		if (sum){
			for (auto cell: m_voxelData.keyList){
				int sumAbundance = 0;
				float sumBiomass = 0;
				const std::map<std::string, int>& cellAbundance = abundance.at(cell);
				const std::map<std::string, float>& cellBiomass = biomass.at(cell);
				for (auto it = cellAbundance.begin(); it != cellAbundance.end(); ++it){
					sumAbundance += abundance.at(cell).at(it->first);
				}
				for (auto it = cellBiomass.begin(); it != cellBiomass.end(); ++it){
					sumBiomass += biomass.at(cell).at(it->first);
				}
				summedAbundance.setValue(cell, sumAbundance);
				summedBiomass.setValue(cell, sumBiomass); 
			}
		}

		if (ordered){
			nlohmann::ordered_json orderedAbundance;
			nlohmann::ordered_json orderedBiomass;
			for (auto cell: m_voxelData.keyList){
				orderedAbundance[cell.to_string()] = abundance.at(cell);
				orderedBiomass[cell.to_string()] = biomass.at(cell);
			}
            std::ofstream fileAB(m_voxelData.savingDir + fileNameAB);
            fileAB << orderedAbundance.dump(4);
            fileAB.close();
            
            std::ofstream fileBM(m_voxelData.savingDir + fileNameBM);
            fileBM << orderedBiomass.dump(4);
            fileBM.close();

			if (sum){
				nlohmann::ordered_json orderedSummedAbundance;
				nlohmann::ordered_json orderedSummedBiomass;
				for (auto cell: m_voxelData.keyList){
					orderedSummedAbundance[cell.to_string()] = summedAbundance.at(cell);
					orderedSummedBiomass[cell.to_string()] = summedBiomass.at(cell);
				}
				std::ofstream fileSumAB(m_voxelData.savingDir + fileNameSumAB);
				fileSumAB << orderedSummedAbundance.dump(4);
				fileSumAB.close();
				std::ofstream fileSumBM(m_voxelData.savingDir + fileNameSumBM);
				fileSumBM << orderedSummedBiomass.dump(4);
				fileSumBM.close();
			}
		} else {
			assert(false && "not implemented yet");
		}
	}
	else {LOG(DEBUG) << "not saving";}
}

// void PlantModel::createInputMaps(const Landscape<std::string>& soils, const Landscape<std::map<std::string, double>>& disturbance, bool init){
// 	LOG(INFO) << "Creating habitat suitability";
// 	if (m_plantInputs_ptr->config.doesSoilClass) {
// 		assert (soils.getTotncell() == m_cells.getTotncell()); //quick check. should probably better check all keys here.
// 		for (auto cell: m_cells) {
// 			cell.second->CreateHabSuit(init, soils.at(cell.first));}; //at initialization it will killExisting plants in !suitable cells
// 	} else {
// 		for (auto cell: m_cells) {
// 			cell.second->CreateHabSuit(init);};
// 	}
// 	if (m_plantInputs_ptr->config.doesDisturbance){
// 		assert(disturbance.getTotncell() == m_cells.getTotncell());  //quick check. should probably better check all keys here.
// 		LOG(INFO) << "Creating disturbance";
// 		for (auto cell: m_cells) {
// 			cell.second->CreateDisturbances(disturbance.at(cell.first));};
// 	}
// }