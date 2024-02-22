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
 // RFate team (RFATE)
 // JJ: partial reimplementation, addition of some new variables (EPM)
 // --------------------------------------------------------------------------

#include "PFG.h"
#include "FGUtils.h"


/** @cond */
#include "nlohmann/json.hpp"
#include <vector>
#include <string>
#include "easylogging++.h"
#include <string_view>
/** @endcond */


PFG::PFG(std::string_view name, const int m, const int l, const Abund& maxa, const double& immsize, const int maxstratum,
	const std::vector<int>& strata, const std::vector<int>& pooll, 
	const int potentialfecundity, const int lightshadefactor,
	const std::vector<Fract>& lightactivegerm, const std::vector<std::vector<bool>>& lighttolerance, 
	const std::vector<std::string>& soiltol, const int depthReq):
	name(name), M(m), L(l), MaxA(maxa), ImmSize(immsize), MaxStratum(maxstratum), Strata(strata), PoolL(pooll),
	potentialFecundity(potentialfecundity), LightShadeFactor(lightshadefactor), LightActiveGerm(lightactivegerm),
	LightTolerance(lighttolerance), soilTol(soiltol), DepthReq(depthReq)
	{}


PFG::PFG(const nlohmann::json& pfg){
	try  {name = pfg.at("Name");} catch(...){LOG(FATAL) << "please provide a name";}
	
	try {M = pfg.at("Maturation_time");} 
		catch(nlohmann::json::out_of_range){LOG(FATAL) << "Error in PFG: Maturation_time time not found";}
	try {L = pfg.at("Life_Span");} 
		catch(nlohmann::json::out_of_range){LOG(FATAL) << "Error in PFG: Life_Span Span not found";}

	try {std::string temp0 = pfg.at("Max_Abund");
		//convert string to enum Abund
		if (temp0[0] == 'N') MaxA = ANone;  //only first letter is used, while json file contains full word. Capitalization matters
		else if (temp0[0] == 'L') MaxA = ALow;
		else if (temp0[0] == 'M') MaxA = AMedium;
		else if (temp0[0] == 'H') MaxA = AHigh;
		else throw std::runtime_error("Error in PFG::PFG(nlohmann::json pfg, PFGresponse response) : Abund not recognized");
		} catch(nlohmann::json::out_of_range){LOG(FATAL) << "Error in PFG: Max_Abund not found";}

	try {ImmSize = pfg.at("ImmSize");} 
		catch(nlohmann::json::out_of_range){LOG(FATAL) << "Error in PFG: ImmSize not found";}
	try {MaxStratum = pfg.at("MaxStratum");} 
		catch(nlohmann::json::out_of_range){LOG(FATAL) << "Error in PFG: MaxStratum not found";}

	try {std::vector<int> temp = pfg.at("StrataChangeAge");
		Strata.reserve(temp.size());
		Strata.resize(temp.size());
		for (int i = 0; i < temp.size(); i++) {
			Strata[i] = temp[i];
		}} catch(nlohmann::json::out_of_range){LOG(FATAL) << "Error in PFG: StrataChangeAge not found";}

	try {std::vector<int> temp2 = pfg.at("PoolL");
		PoolL.reserve(temp2.size());
		PoolL.resize(temp2.size());
		for (int i = 0; i < temp2.size(); i++) {
			PoolL[i] = temp2[i];
		}} catch(nlohmann::json::out_of_range){LOG(FATAL) << "Error in PFG: PoolL not found";}

	// m_InnateDorm = pfg.at("InnateDorm");
	try {potentialFecundity = pfg.at("PotentialFecundity");} 
		catch(nlohmann::json::out_of_range){LOG(FATAL) << "Error in PFG: PotentialFecundity not found";}
	try {LightShadeFactor = pfg.at("LightShadeFactor");} 
		catch(nlohmann::json::out_of_range){LOG(FATAL) << "Error in PFG: LightShadeFactor not found";}

	try {std::vector<double> temp3 = pfg.at("LightActiveGerm");
		LightActiveGerm.reserve(temp3.size());
		LightActiveGerm.resize(temp3.size());
		for (int i = 0; i < temp3.size(); i++) {
			LightActiveGerm[i] = DoubleToFract(temp3[i]);
		}} catch(nlohmann::json::out_of_range){LOG(FATAL) << "Error in PFG: LightActiveGerm not found";}

	try {LightTolerance = pfg.at("LightTolerance");} catch(nlohmann::json::out_of_range){LOG(FATAL) << "Error in PFG: LightTolerance not found";}
	try {soilTol = pfg.at("SoilTol");} catch(nlohmann::json::out_of_range){LOG(FATAL) << "Error in PFG: SoilTol not found";}
	try {DepthReq = pfg.at("DepthReq");} catch(nlohmann::json::out_of_range){LOG(FATAL) << "Error in PFG: DepthReq not found";}

	//----------------done.-----------------------
	check();

	// allocation = ResourceAlloc();//testing only
}

void PFG::check(){
	//stricter sanity check during debugging:
	assert(M < 100);
	assert(L < 200);
	assert(MaxStratum <= 4);

	//actual sanity check:
	if (M < 0 || M > 9999) LOG(FATAL) << "Error in PFG: Maturation_time < 0 or > 9999";
	if (L < 0 || L > 9999) LOG(FATAL) << "Error in PFG: Life_Span < 0 or > 9999";
	if (ImmSize <0 || ImmSize > 1.0) LOG(FATAL) << "Error in PFG: Immsize < 0 or > 1: " << std::to_string(ImmSize);

	for (int i = 0; i < Strata.size();i++){
		if (Strata[i]< 0) LOG(FATAL) << "Error in PFG: change-age of stratum <0.";
		if ((i> MaxStratum) && (Strata[i] != 10000)) 			LOG(FATAL) << "Error in PFG: please set the change-age of strata that the plant cannot reach to 10000 (start counting strata at 0).";
		if ((i<=  MaxStratum) && (Strata[i] >L)) LOG(FATAL) << "Error in PFG: change-age of stratum is larger than lifespan";
	}

	if(PoolL.size() != PoolType::PTcount) LOG(FATAL) << "Error in PFG: wrong number of pool types.";
	for (int i = 0; i < PoolL.size();i++){
		if (PoolL[i] < 0 || PoolL[i]>9999) LOG(FATAL) << "Error in PFG: life span of pool " << std::to_string(i) << " is " + std::to_string(PoolL[i]);
	}
	//more here

	if (LightTolerance.size() != LifeStage::LScount) LOG(FATAL) << "Error in PFG: LightTolerance should be given for each life stage, but is not of size " << std::to_string(static_cast<int>(LifeStage::LScount));
	for (int i = 0; i < LifeStage::LScount; i++) {
		if (LightTolerance[i].size() != Resource::Rcount) LOG(FATAL) << "Error in PFG: LightTolerance should be given for each light level, but is in stage" << std::to_string(i) << "not of size " + std::to_string(static_cast<int>(Resource::Rcount));
	}

}
