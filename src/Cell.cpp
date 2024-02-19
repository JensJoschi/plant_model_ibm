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
 // RFate team
 // JJ: added shading, complete restructuring and cleanup; calculate light passing through neighbour cells
 // ---------------------------------------------------------------------------


#include "Cell.h"
#include "GSP_PLANTS.h"

#include "FuncGroup.h"
#include "PFGDefs.h"
#include "PFG.h"
#include "FGUtils.h"

/** @cond */
#include <vector>
#include <string>
#include <map>
#include "easylogging++.h"
#include <string_view>
/** @endcond */

#include "rng.h"


//==============================================================================
//public

Cell::Cell(const GSP_PLANTS* const gsp_plants, const PFGDefs* pfgs,
	const double shade, const int depth, const std::vector<int>& commSize, const int poolSize):
	m_gsp_ptr(gsp_plants), m_shading(shade), m_depth(depth), m_LightR(gsp_plants->noStrata, RHigh){
			
			assert(commSize.size() == 4);
			m_funcGroups.reserve(pfgs->size());
			for (auto i : pfgs->getNames()){
				m_funcGroups.push_back(FuncGroup(&pfgs->getPFG(i), &pfgs->getDisturbance(i), commSize, poolSize, m_gsp_ptr->maxAbundLow, m_gsp_ptr->maxAbundMedium, m_gsp_ptr->maxAbundHigh));
			}
			m_EnvUnsuitability.resize(m_funcGroups.size());
}

/*----------------------------------------------------------------------------*/

void Cell::CreateHabSuit(bool killExisting, std::string_view soil){
	assert(m_funcGroups.size()>0);
	m_EnvSuitRefVal = UpdateEnvSuitRefVal();
	for (auto i = 0; i< m_funcGroups.size(); ++i){
		bool suitable = (!m_gsp_ptr->doesSoilDepth || m_funcGroups[i].checkSoilDepth(m_depth)) &&
			(!m_gsp_ptr->doesSoilClass || m_funcGroups[i].checkSoilClass(soil));
		m_EnvUnsuitability[i] = !suitable;

		if(killExisting && !suitable){
			m_funcGroups[i].remove();
		}
	}

}

void Cell::CreateDisturbances(const std::map<std::string, double>& dists){
	assert(dists.size()>0);
	if (m_disturbances.size() == 0){
		m_disturbances = dists;
	} else {
		for (auto it = dists.begin(); it != dists.end(); ++it){
			m_disturbances[it->first] = it->second;
		}
	}
}

std::map<std::string, int> Cell::DoSuccession(const std::map<std::string, int>& seeds_in){
	const std::vector<int> neighbour(m_gsp_ptr->noStrata, 0);
	return(DoSuccession(seeds_in, neighbour));
}
std::map<std::string, int> Cell::DoSuccession(const std::map<std::string, int>& seeds_in, const std::vector<int>& neighbour){
	assert(neighbour.size() == m_gsp_ptr->noStrata);
	unsigned noFG = m_funcGroups.size();
	std::map<std::string, int> SeedProd;  			//stores the number of seeds produced by each FG
	for (auto it = seeds_in.begin(); it != seeds_in.end(); ++it){
		SeedProd.insert(std::pair<std::string, int>(it->first, 0)); //copies only names!
	}

	if (noFG > 0){
		// 0. Apply disturbances
		if (m_gsp_ptr->doesDisturbance){
			for (unsigned fg = 0; fg<noFG; fg++){
				m_funcGroups.at(fg).beDisturbed(m_disturbances);}
		}

		// 1. Kill plants which cannot tolerate current light availability
		for (unsigned fg =0; fg<noFG; fg++){
			m_funcGroups.at(fg).die(m_LightR);
			}	

		// 2. Age cohorts and legions
		for (unsigned fg = 0; fg<noFG; fg++){
			m_funcGroups.at(fg).age();}

		// 3. Recalculate the environment
		updateLight(neighbour, m_gsp_ptr->lightAngle);

		// 4. Calculate germination and recruitment
		int countme = 0;
		//m_funcGroups should become a map. until then it stays ugly.
		for (auto foo : seeds_in){
			std::string fg = foo.first;
			m_funcGroups.at(countme).germinateAndRecruit(m_gsp_ptr, seeds_in.at(fg), m_LightR.at(0), m_EnvUnsuitability[countme]);
			countme++;
		} 

		// 5. Calculate fecundity
		countme = 0;
		for (auto foo : seeds_in){
			std::string fg = foo.first;
			SeedProd[fg] = std::max(0, m_funcGroups.at(countme).getFecund());
			countme++;
		}
	}
	return(SeedProd);
}



/*----------------------------------------------------------------------------*/


int Cell::getPFGabund(const int pfg) {
	assert(m_funcGroups.size()>0 && pfg < m_funcGroups.size());
	return(m_funcGroups.at(pfg).getCount());
}


//==============================================================================
//private

double Cell::UpdateEnvSuitRefVal(){
	std::uniform_real_distribution<> ls{ 0.0, 1.0};
    double envSuitRefVal = ls(RNGs::mersenne);
  return (envSuitRefVal);
}


void Cell::updateLight(){
	const std::vector<int> unused = calcAbundances();
	assert(unused.size() == m_gsp_ptr->noStrata);
	updateLight(unused, 90); 
} 

void Cell::updateLight(const std::vector<int>& abundanceNeighbour, const int angle){
	assert(m_shading >= 0.0 && m_shading <= 1.0);
	assert(abundanceNeighbour.size() == m_gsp_ptr->noStrata);
	assert(m_gsp_ptr->strataHeight.size() == m_gsp_ptr->noStrata);
	std::vector<int> cellAbundances = calcAbundances();
	assert(cellAbundances.size() == m_gsp_ptr->noStrata);
	
	/* Work down the strata calculating */
	int XAbove = 0;
	m_LightR[m_gsp_ptr->noStrata-1] = RHigh;
	for (int Stm = m_gsp_ptr->noStrata-2; Stm > -1; Stm--){//with 4 strata, start at stratum "2" (which is the third one -0 ,1,2)
		XAbove = XAbove + calculateLight(cellAbundances[Stm+1], angle, abundanceNeighbour[Stm+1], m_gsp_ptr->strataHeight[Stm+1]); //here the function does not actually calculate light, but shade
		if (XAbove < m_gsp_ptr->lightThreshMedium * (1-m_shading)){
			m_LightR[Stm] = RHigh;
		} else if (XAbove < m_gsp_ptr->lightThreshLow * (1-m_shading)){
			m_LightR[Stm] = RMedium;
		} else{
			m_LightR[Stm] = RLow;
		}
	}
} 

int Cell::calculateLight(const int lightAbove, const int angle, const int lightNeighbour, const int height){
	assert(m_shading >= 0.0 && m_shading <= 1.0);
	assert(angle > 0 && angle <= 90);
	assert(lightAbove >= 0 && lightNeighbour >= 0);
	float x = std::min(1.0, height/100.0 * 1.0/tan(static_cast<float>(angle) * M_PI / 180.0));
	float y = 1.0-x;
	return static_cast<int> (lightNeighbour * x + lightAbove * y);
} 


std::vector<int> Cell::calcAbundances(){
	std::vector< int > stProfile(m_gsp_ptr->noStrata, 0);
	for (int fg=0; fg < m_funcGroups.size(); fg++){
		for (unsigned st=0; st<stProfile.size(); st++){
			stProfile[st] += m_funcGroups[fg].getStratAbund(st);
		}
	}
	return stProfile;;
}
