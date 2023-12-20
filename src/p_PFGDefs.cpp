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

 // --------------------------------------------------------------------------
 // Authors and contributors to this file:
 // RFate team: original idea
 // Jens Joschinski: reimplementation
 // --------------------------------------------------------------------------

#include "p_PFGDefs.h"
#include "p_PFG.h"
#include "p_PFGdisturbance.h"

/** @cond */
#include <vector>
#include <map>
#include <string>
#include "easylogging++.h"
#include <ranges>
/** @endcond */

using std::vector;
using std::string;

//==============================================================================
//Constructors
//==============================================================================

PFGDefs::PFGDefs(){
	//this "clover" has actuall attributes of a medium-sized bush that is half shade-tolerant. It gets its name to match with the animal model.
	PFG Tree("clover", 2,6, Abund(AMedium), 1,4, vector{ 0, 5,10, 13, 10000, 10000, 10000, 10000}, vector{1,0}, //false,
		1,5, vector<Fract>{PC10, PC50, PC90}, vector<vector<bool>>{{false,false,true}, {false,false,true}, {false,false,true}, {false,false,true}},// 1, 1,1,10,
		vector<string> {"Lo_l_2","ClLo_l_2","Cl_l_2","Cl_l_3","SaClLo_l_3","ClLo_l_3","SaClLo_l_2","SaLo_l_2","SaClLo_l_1","LoSa_l_2",
		"Cl_l_1","Cl_h_3","SaLo_h_3","Lo_l_1","ClLo_h_3","SiLo_1","SiClLo_1","SiClLo_2","SaClLo_h_2","SaLo_l_1","SiLo_2","Lo_h_1",
		"Lo_h_2","Cl_h_2","Lo_h_3","SiClLo_3","SaLo_h_2","LoSa_l_3","ClLo_h_2","ClLo_l_1","Cl_h_1","LoSa_l_1","Lo_l_3","SaLo_l_3",
		"SiLo_3","SaClLo_h_3","SaLo_h_1","LoSa_h_3","LoSa_h_2","LoSa_h_1","SaClLo_h_1","ClLo_h_1"}, 2);

	PFG MedBush("clover2", 12,100, Abund(AMedium), 0.6,1, vector{ 0, 5,10, 13, 10000, 10000, 10000, 10000}, vector{1,0}, //false,
		1,5, vector<Fract>{PC10, PC50, PC90}, vector<vector<bool>>{{false,false,true}, {false,false,true}, {false,false,true}, {false,false,true}},/// 1, 1,1,10,
		vector<string> {"Lo_l_2","ClLo_l_2","Cl_l_2","Cl_l_3","SaClLo_l_3","ClLo_l_3","SaClLo_l_2","SaLo_l_2","SaClLo_l_1","LoSa_l_2",
		"Cl_l_1","Cl_h_3","SaLo_h_3","Lo_l_1","ClLo_h_3","SiLo_1","SiClLo_1","SiClLo_2","SaClLo_h_2","SaLo_l_1","SiLo_2","Lo_h_1",
		"Lo_h_2","Cl_h_2","Lo_h_3","SiClLo_3","SaLo_h_2","LoSa_l_3","ClLo_h_2","ClLo_l_1","Cl_h_1","LoSa_l_1","Lo_l_3","SaLo_l_3",
		"SiLo_3","SaClLo_h_3","SaLo_h_1","LoSa_h_3","LoSa_h_2","LoSa_h_1","SaClLo_h_1","ClLo_h_1"}, 11);

	m_PFGs.insert(std::pair<string, PFG>("clover", Tree));
	m_PFGs.insert(std::pair<string, PFG>("clover2", MedBush));
	fillDisturbances();
	check();
	}



PFGDefs::PFGDefs(const std::string& pathtodefs, const std::string& pathtodisturbances){
	createPFGs(pathtodefs);
	createDisturbances(pathtodisturbances);
	fillDisturbances();
	check();
}


PFGDefs::PFGDefs(const string& pathtodefs): m_Disturbances() {
	createPFGs(pathtodefs);
	fillDisturbances();
		check();
}

PFGDefs::PFGDefs(const std::map<string, PFG>& pfgin,  const std::map <std::string, PFGDisturbances>& distin):
	m_PFGs(pfgin), m_Disturbances(distin){
		fillDisturbances();
		check();
	}
//============================================================================== 
//member functions
//==============================================================================


int PFGDefs::size() const {return m_PFGs.size();}

vector<string> PFGDefs::getNames() const{
	auto keys = std::views::keys(m_PFGs);
	vector<string> names{ keys.begin(), keys.end() };
	return names;
}

const std::map<std::string, PFG>& PFGDefs::getPFGs() const {return m_PFGs;}
const std::map<std::string, PFGDisturbances>& PFGDefs::getDisturbances() const {return m_Disturbances;}


const PFG& PFGDefs::getPFG(const std::string& name) const {
	assert(name != "");
	return m_PFGs.at(name);}

const PFGDisturbances& PFGDefs::getDisturbance(const std::string&  name) const {
	assert(name != "");
	return m_Disturbances.at(name);}



void PFGDefs::createPFGs(const string& pathtodefs){
	CLOG(DEBUG, "PLANTS") << "reading PFG definitions from file";
	std::ifstream ifs(pathtodefs);
		if (!ifs.good()){
		LOG(FATAL) << "error: PFGDefs file not found. Path was: " << pathtodefs;
	}
	nlohmann::json j;
	ifs >> j;
	for (auto pfg : j){
		try{ m_PFGs.insert(std::pair<string, PFG>(pfg.at("Name"), PFG(pfg))); }
		catch (const std::runtime_error& e){
			LOG(WARNING) << e.what();
		 	LOG(FATAL) << "error in PFG definition" << pfg.at("Name") << ". Check your json file.";
		}
	}
	ifs.close();
}

void PFGDefs::createDisturbances(const std::string& pathtodisturbances){
	CLOG(DEBUG, "PLANTS") << "reading disturbance definitions from file";
	std::ifstream ifs2(pathtodisturbances);
	if (!ifs2.good()){
		LOG(FATAL) << "error: DisturbanceDefs file not found. Path was: " << pathtodisturbances;
	}
	nlohmann::json j2;
	ifs2 >> j2;
	for (auto dist : j2){
		try{ m_Disturbances.insert(std::pair<std::string, PFGDisturbances>(dist.at("Name"), PFGDisturbances(dist))); }
		catch (const std::runtime_error& e){
			LOG(WARNING) << e.what();
			LOG(FATAL) << "error in Disturbance definition" << dist.at("Name") << ". Check your json file.";
		}
	}
}

void PFGDefs::fillDisturbances(){
	for (auto pfg : m_PFGs){
		if (!m_Disturbances.contains(pfg.first)){
			m_Disturbances.insert(std::pair<std::string, PFGDisturbances>(pfg.first, PFGDisturbances()));
		}
	}
}

void PFGDefs::check(){
	if (m_PFGs.size() == 0){
		LOG(FATAL) << "error: no PFGs defined";
	}
	if (m_Disturbances.size() != m_PFGs.size()){
		LOG(FATAL) << "error: number of PFGs and Disturbances do not match";
	}
}