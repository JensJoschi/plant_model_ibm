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
 // RFate team: provide original code
 // JJ: split SimulMap into a PlantModel class and a EcolopesLand class (different project); 
 //     rewrote habitat suitability and disturbance functions
 //     functions to exchange data with other models
 // VMVC: functions to exchange data with other models
 // ----------------------------------------------------------------------------

#include "p_PlantModel.h"

#include "Landscape.h"
#include "g_Inputs.h"
#include "g_GSP_PLANTS.h"
#include "g_Data_PLANTS.h"
#include "rng.h"


#include "p_PFG.h"
#include "p_Cell.h"
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
//public

PlantModel::PlantModel(const std::string& inputFile, bool FixRNG){
	GSP_PLANTS config{inputFile};
	Data_PLANTS data{inputFile, config};
	m_plantInputs_ptr = new const Inputs_P(config, data);

	std::string WD = def_WD;

	assert(m_plantInputs_ptr);
	std::vector<std::string> keys = m_plantInputs_ptr->data.keyList.getKeys();
	assert(m_plantInputs_ptr->checkKeys());
	assert(m_plantInputs_ptr->config.noStrata != 0); //checks that GSP wasn't default-constructed

	LOG(INFO) << "Building Plant model";

	if (FixRNG){
    LOG(INFO) << "fix RNG with seed 2230";
    RNGs::mersenne = std::mt19937{2230};
  }

	//-----------------------------------------------------------------------------
	const std::vector<std::string>& RegionalModel= m_plantInputs_ptr->data.listPlantFunctionalGroups;
	assert(RegionalModel.size()>0); 

	LOG(DEBUG) << "no. plant FGs:" << RegionalModel.size();
	LOG(INFO) << "--create data structures";

	// if submodels are switched on, their variables get zero-initialized. Otherwise they are default-intialized(empty, not 0).
	if (m_plantInputs_ptr->config.doesPlantDispersal){
		for (int i = 0; i< RegionalModel.size(); i++){
			m_SeedMap.emplace(RegionalModel.at(i), Landscape<int>(keys));
		}
	}

	std::map<std::string, Cell* > cellModels;
	for (auto it = keys.begin(); it != keys.end(); ++it){
		//use inputs or make inputs 0, depending on doesSubModel entries in config
		double shading = m_plantInputs_ptr->config.doesShadingPercentages ? m_plantInputs_ptr->data.shading.at(*it) : 0.0;
		int depth = m_plantInputs_ptr->config.doesSoilDepth ? m_plantInputs_ptr->data.soilDepth.at(*it) : 0;

				cellModels.emplace(*it, new Cell(&m_plantInputs_ptr->config, 
					&m_plantInputs_ptr->data.PFGDefinitions, shading, depth,
					std::vector<int>{100,0,1,1}, 10));
	}

	m_cells = Landscape <Cell*> (cellModels);
}

//--------------------------------------------------------------------------

void PlantModel::initialize(int years){
	assert(years>=0);

	const Landscape<std::string>& HabSuitMap = m_plantInputs_ptr->config.doesSoilClass? 
		m_plantInputs_ptr->data.soilClass : Landscape<std::string>(m_plantInputs_ptr->data.keyList.getKeys());
    const Landscape<std::map<std::string, double>>& DisturbanceMap =  m_plantInputs_ptr->config.doesDisturbance?
        m_plantInputs_ptr->data.management : Landscape<std::map<std::string, double>>(m_plantInputs_ptr->data.keyList.getKeys());
	createInputMaps(HabSuitMap, DisturbanceMap, true);

	LOG(INFO) << "let the plants grow for " << years << " years ";
	for (int i =0; i< years; i++){
		LOG(DEBUG) << "year: " << i;
		TPlusOne_JJ();
	}
	LOG(DEBUG) << "done.";
}


void PlantModel::TPlusOne_JJ(){
	LOG(INFO) << "--Succession";

	if (!m_plantInputs_ptr->config.doesNeighbourinfluence) {
		for (auto cell: m_cells) Dosuccession(cell.first);
	} else{
		std::vector<int> neighbour( m_plantInputs_ptr->config.noStrata, 0);  //for testing, a fake neighbouring cell
		for (auto cell: m_cells){
			std::string IDent = cell.first;
			int x = std::stoi(IDent.substr(1, IDent.find(",")-1)) - 1;
			std::string neighbourcell = "(" + std::to_string(x) + IDent.substr(IDent.find(","));
			if (m_cells.contains(neighbourcell)){
				neighbour = m_cells.at(neighbourcell)->calcAbundances();}
			else {neighbour = std::vector<int>(m_plantInputs_ptr->config.noStrata, 0);}

		 	DoSuccession(cell.first, neighbour); 
		}
	}
	//this seems like an ideal place for parallelization. Instead of the lines above we could do:
	// std::vector<std::string> cellnames;
	// 	for (auto cell : mask){ 
	// 	cellnames.push_back(cell.first);
	// 	}
	// 	#pragma omp parallel for
	// 		for (int i=0; i<cellnames.size();i++){
	// 			if (m_plantInputs_ptr->config.doesAnimalDisturbance){disturbAnimals(cellnames[i]);}
	// 			DoSuccession(cellnames[i]);
	// 		}
	//parallelization does not seem to win any time here, probably because of the small number of cells. 
	//Need to test again later with a larger sample
	LOG(INFO) << "--Dispersal";
	if ( m_plantInputs_ptr->config.doesPlantDispersal) {disperse();};
}


//--------------------------------------------------------------------------

void PlantModel::createInputMaps(const Landscape<std::string>& soils, const Landscape<std::map<std::string, double>>& disturbance, bool init){
	LOG(INFO) << "Creating habitat suitability";
	if (m_plantInputs_ptr->config.doesSoilClass) {
		assert (soils.getTotncell() == m_cells.getTotncell()); //quick check. should probably better check all keys here.
		for (auto cell: m_cells) {
			cell.second->CreateHabSuit(init, soils.at(cell.first));}; //at initialization it will killExisting plants in !suitable cells
	} else {
		for (auto cell: m_cells) {
			cell.second->CreateHabSuit(init);};
	}
	if (m_plantInputs_ptr->config.doesDisturbance){
		assert(disturbance.getTotncell() == m_cells.getTotncell());  //quick check. should probably better check all keys here.
		LOG(INFO) << "Creating disturbance";
		for (auto cell: m_cells) {
			cell.second->CreateDisturbances(disturbance.at(cell.first));};
	}
}

void PlantModel::savePFG(int id, std::string fileName){
	assert(id >= 0);
	assert(id < m_plantInputs_ptr->data.listPlantFunctionalGroups.size());
	if (fileName == "") fileName = m_plantInputs_ptr->data.listPlantFunctionalGroups.at(id) + ".json";
	Landscape<double> abundances = getPFGabund(id);
	abundances.write_json(m_plantInputs_ptr->data.savingDir + fileName);

}

void PlantModel::savePFG(const std::string& pfg, std::string fileName){
	assert(pfg != "");
	if (fileName == "") fileName = pfg + ".json";
	const std::vector<std::string>& names = m_plantInputs_ptr->data.PFGDefinitions.getNames();
	for (int i = 0; i < names.size(); i++){
		if (names.at(i) == pfg){
			savePFG(i, fileName);
			return;
		}
	}
	LOG(ERROR) << "Error when saving. PFG " << pfg << " not found";
}

void PlantModel::savePFG(const std::string& pfg, int year, std::string fileName){
	if (fileName == "") fileName = pfg + "_" + std::to_string(year) + ".json";
	auto& saveYears = m_plantInputs_ptr->config.m_saveYears;
	if (std::find(saveYears.begin(), saveYears.end(), year) != saveYears.end()){
		LOG(INFO) << "SAVING";
		savePFG(pfg, fileName);
	}
	else {LOG(DEBUG) << "not saving";}
}

void PlantModel::saveAll(int year, std::string fileName){
	auto& saveYears = m_plantInputs_ptr->config.m_saveYears;
	if (std::find(saveYears.begin(), saveYears.end(), year) != saveYears.end()){
		if (fileName == "") fileName = "biomass_" + std::to_string(year) + ".json";
		LOG(INFO) << "SAVING";
		saveAll(fileName);
	}
	else {LOG(DEBUG) << "not saving";}
}

void PlantModel::saveAll(std::string fileName){
	if (fileName == "") fileName = "biomass.json";
	Landscape<std::map<std::string,double>> biomass(m_plantInputs_ptr->data.keyList.getKeys());
	// for (const auto& pfg: m_plantInputs_ptr->data.listPlantFunctionalGroups){
		//for (auto cell){... getPFGabund(pfg);} //does not work yet because getPFGabund uses int
	// }
	//use int-based for loop instead:
	int noPFGs = m_plantInputs_ptr->data.listPlantFunctionalGroups.size();
	for (int i = 0; i < noPFGs; i++){
		const std::string& pfgName = m_plantInputs_ptr->data.listPlantFunctionalGroups.at(i);
		for (auto cell = m_cells.begin(); cell != m_cells.end(); ++cell){
			biomass.insertValue(cell->first, pfgName, cell->second->getPFGabund(i));
		}	
	}
	biomass.write_json(m_plantInputs_ptr->data.savingDir + fileName);	
}

//==============================================================================
//private
void PlantModel::Dosuccession(std::string cell){
	const std::vector<int> unused(m_plantInputs_ptr->config.noStrata, 0);
	DoSuccession(cell, unused);
}
void PlantModel::DoSuccession(std::string cell, const std::vector<int>& neighbour){
		assert(m_SeedMap.size() == m_plantInputs_ptr->data.listPlantFunctionalGroups.size());
		assert(cell != "");
		assert(neighbour.size() == m_plantInputs_ptr->config.noStrata);
		// seedMap format: {PFG1: {(0,0,0): 5, (0,0,1): 10...}, PFG2: {(0,0,0): 8, (0,0,1): 9...}, ...}
		// succession model needs {(0,0,0): {PFG1: 4, PFG2: 5, ...}} -> conversion necessary
		std::map<std::string, int> seeds_in;  //{clover: 4, grass: 5}
		for(auto it = m_SeedMap.begin(); it != m_SeedMap.end(); ++it){
			std::string fg = it->first;
			seeds_in.insert(std::pair<std::string, int>(fg, it->second.at(cell)));
		}

		std::map<std::string, int> seeds_out = m_cells.at(cell)-> DoSuccession(seeds_in, neighbour);
		//convert back:
		for (auto it = m_SeedMap.begin(); it != m_SeedMap.end(); ++it){
			it->second.setValue(cell, seeds_out.at(it->first));
		}

}

void PlantModel::disperse(){
	std::map<std::string, Landscape <int>> newSeeds = DoDispersalPacket(m_SeedMap, (m_plantInputs_ptr->config.seedInput));
	m_SeedMap = newSeeds;
}

std::map<std::string, Landscape<int>> PlantModel::DoDispersalPacket(const std::map<std::string, Landscape<int>>& m_SeedMapIn, int regionalSeeds){
	std::map<std::string, Landscape<int>> m_SeedMapOut;

	std::uniform_int_distribution<> ls{ 0, static_cast<int>(m_SeedMapIn.begin()->second.getTotncell() - 1 )};

	//loop over FGs in m_SeedMapIn
	for(auto const& f: m_SeedMapIn){ 
		std::string pfg = f.first;

		//get total seeds for this PFG
		int total = 0;
		for(auto const& cell: f.second){
			total += cell.second;
		}
		total += regionalSeeds * f.second.getTotncell();

		//create vector of cells where the seeds could land (currently everywhere on site)
		std::vector<std::string> keys;
		for(auto const& cell: f.second){
			keys.push_back(cell.first);
		}

		//place seeds randomly on site
		Landscape<int> temp(keys);
		for (int s = 0; s < total; s++){
			const std::string& key = keys[ls(RNGs::mersenne)]; //pick random key
			temp.setValue(key, temp.at(key) + 1); //ad 1 seed to this cell
		}
		m_SeedMapOut.insert(std::pair<std::string, Landscape<int>> (pfg , temp));
	} //next FG
	return m_SeedMapOut;
}



//--------------------------------------------------------------------------

Landscape<double> PlantModel::getPFGabund(int PFG_id){
	std::map<std::string, double> abundances;
	for (auto it : m_cells){
		abundances[it.first] = (static_cast<double> (it.second->getPFGabund(PFG_id)));
	}
	return (Landscape<double>(abundances));
}