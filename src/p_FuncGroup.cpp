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
 // RFate team
 // JJ complete restructuring and cleanup; reimplementation of disturbance and soil checks
 // --------------------------------------------------------------------------


#include "p_FuncGroup.h"

#include "g_GSP_PLANTS.h"

#include "p_PropPool.h"
#include "p_PFG.h"
#include "g_FGUtils.h"
#include "p_Cohort.h"
#include "p_PFGdisturbance.h"

#include "rng.h"
/** @cond */
#include <vector>
#include <string>
#include "easylogging++.h"
#include <string_view>
#include <random>
/** @endcond */

//==============================================================================
//public:
FuncGroup::FuncGroup(): m_FGparams_ptr(nullptr), m_FGdist_ptr(nullptr), m_pools(std::vector<PropPool> { PropPool(0, false, 0), PropPool(0, true, 0)}), m_abundLimit(0){}

FuncGroup::FuncGroup(const FuncGroup& other): 
	m_FGparams_ptr(other.m_FGparams_ptr), m_FGdist_ptr(other.m_FGdist_ptr), m_pools(other.m_pools), m_cohorts(other.m_cohorts), m_abundLimit(other.m_abundLimit){}

FuncGroup::FuncGroup(const PFG* pfg, const PFGDisturbances* disturbance, const std::vector<int>& cohortSize, const int poolSize,
	const int low, const int med, const int high): //to do: pack these into a vector
		m_FGparams_ptr(pfg), m_FGdist_ptr(disturbance),
		m_pools(std::vector<PropPool> { PropPool(0, false, 0), PropPool(poolSize, true, 0)}),
		m_abundLimit(std::vector<int> {low, med, high}[pfg->MaxA-1]){
			//very simple way to create random starting distribution (random abundances)
			std::uniform_int_distribution<> dis(0, cohortSize[0]);
			
			assert (cohortSize.size() == 4); //note that cohorts[3] is silently ignored!
			assert(poolSize>= 0);
			int r = dis(RNGs::mersenne);
			r = dis(RNGs::mersenne);
			m_cohorts.reserve(1);
			m_cohorts.push_back(Cohort{r, cohortSize[1], cohortSize[2]});
}

FuncGroup::FuncGroup(const PFG* pfg, const PFGDisturbances* disturbance, const std::vector<int>& cohortSize, const int poolSize,
	const int low, const int med, const int high, int n): //to do: pack these into a vector
		m_FGparams_ptr(pfg), m_FGdist_ptr(disturbance),
		m_pools(std::vector<PropPool> { PropPool(0, false, 0), PropPool(poolSize, true, 0)}),
		m_abundLimit(std::vector<int> {low, med, high}[pfg->MaxA-1]){
			//very simple way to create random starting distribution (random abundances)
			std::uniform_int_distribution<> dis(0, cohortSize[0]);
			
			assert (cohortSize.size() == 4); //note that cohorts[3] is silently ignored!
			assert(poolSize>= 0);
			int r = dis(RNGs::mersenne);
			r = dis(RNGs::mersenne);
			m_cohorts.reserve(1);
			m_cohorts.push_back(Cohort{n, cohortSize[1], cohortSize[2]});
}


/*----------------------------------------------------------------------------*/
//main functions

void FuncGroup::germinateAndRecruit(
	const GSP_PLANTS* const gsp, const int seeds_in, const Resource& R0, const bool affected_by_env ){
	assert(seeds_in >= 0);
	assert(R0 >= 0 && R0 < Rcount);
	assert(gsp != nullptr);
	assert(m_FGparams_ptr);
	PropPool* App_ptr = &m_pools.at(PoolType::ActiveP);
	PropPool* Dpp_ptr = &m_pools.at(PoolType::DormantP);

	//1. calculate available seeds (seed pool, regional model, and dispersed seeds) 
	int SeedInput = seeds_in;
	int AvailSeeds = App_ptr->getSize() + SeedInput;
	
	//2. calculate germination rate
	const std::vector<int> abunds{gsp->maxAbundLow,gsp->maxAbundMedium, gsp->maxAbundHigh};
	double GerminRate = static_cast<double>(std::min(abunds[static_cast<int>(m_FGparams_ptr->MaxA)-1], AvailSeeds)) ; 
	// if (gsp->doesLightInteraction){
		assert(m_FGparams_ptr->LightActiveGerm[R0] < Fract::Fcount);
		GerminRate *= FractToDouble(m_FGparams_ptr->LightActiveGerm[R0]);
	// }
	/* 3. put inactive seeds back */
	App_ptr->EmptyPool();
	App_ptr->PutSeedInPool(AvailSeeds - GerminRate);

	/* 4. Recruitment */
	// if (!gsp->doesLightInteraction) doRecruitment(AvailSeeds, gsp, R0, affected_by_env);
	// else{
		if (m_FGparams_ptr->LightTolerance[LifeStage::Germinant].at(R0)){
			doRecruitment(GerminRate, gsp, R0, affected_by_env);
		} else{} //die
	// }  
	// 5. age inactive seeds
	//seed dormancy is deactivated for now. I suggest first rewriting the PropPool class. 
	App_ptr->AgePool1(m_FGparams_ptr->PoolL[PoolType::ActiveP]);
	// if (m_FGparams_ptr->innateDormancy){Dpp_ptr->AgePool1((m_FGparams_ptr->PoolL[PoolType::ActiveP]));}
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

void FuncGroup::age(int maxAge){
	assert(maxAge >= -1);
	if (maxAge == -1) maxAge = m_FGparams_ptr->L;

	if (m_cohorts.size() > 0){
		for (unsigned i=0; i<m_cohorts.size(); i++){
			m_cohorts[i].Ay += 1;
			m_cohorts[i].Ao = std::min( m_cohorts[i].Ao + 1,  maxAge);  //JJ Ao cannot exceed maxAge!
		}
		/* check that maximum age has not been exceeded in the last cohort */  //<<<<-----  JJ: are the cohorts guaranteed to be sorted by Age??
		/* if it is the case, remove the last cohort */
		if (m_cohorts.back().Ay >  m_cohorts.back().Ao){
			m_cohorts.pop_back();
		}
	}
}
//potentially better version:
// void FuncGroup::age(){
// 	for (unsigned i=0; i<m_cohorts.size(); i++){
// 		m_cohorts[i].Ay += 1;
// 		if (m_cohorts[i].Ay >  m_FGparams_ptr->L){
// 			m_cohorts.erase(m_cohorts.begin()+i); // in contrast to above, this version does not mind the sorting. If indeed at the end of the vector, there is no performance diff
// 		} else{
// 			m_cohorts[i].Ao = std::min( m_cohorts[i].Ao + 1,  m_FGparams_ptr->L);
// 		}
// 	}
// }



/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

void FuncGroup::die(const std::vector<Resource>& light){
	if (m_cohorts.size()== 0){ return; }

	/* Fill the stratum according to Cohorts of each PFG */
	int co = 0;
	while (co < m_cohorts.size()){
		int ayTemp = m_cohorts[co].Ay;
		int aoTemp = m_cohorts[co].Ao;

		/* Get the first stratum which  cohort filled */
		int st = 0;
		assert(m_FGparams_ptr->Strata.size() == light.size());
		while (ayTemp >= m_FGparams_ptr->Strata[st+1]) { st++; }

		/* only matures or only immature plants in this cohort */
		if (ayTemp >= m_FGparams_ptr->M || aoTemp <m_FGparams_ptr->M){ 
			/* check if plants are able to survive in this strata */
			bool survival = ayTemp >= m_FGparams_ptr->M ? 
				m_FGparams_ptr->LightTolerance[LifeStage::Mature][light.at(st)] :
				m_FGparams_ptr->LightTolerance[LifeStage::Immature][light.at(st)];

			if (survival){
				if (aoTemp < m_FGparams_ptr->Strata[st+1]) {co++;} /* All Plants are in the same stratum, The whole group survives */
				else{ /* Plants covered more than a lone stratum */
					splitCohort(co, m_FGparams_ptr->Strata[st+1]-1);
					co++;
					}
			} else{  //no survival
				removeCohort(ayTemp, std::min(aoTemp, m_FGparams_ptr->Strata[st+1]-1));
			}
		} else{  /* If there are both immatures and matures plants in this cohort */
			splitCohort(co, m_FGparams_ptr->M -1);
			co++;
		}
	} //end of cohorts
	pickupCohorts();
} 

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/


void FuncGroup::beDisturbed(const std::map<std::string, double>& dist){
	// LOG(DEBUG) << "PFG " << getName();
	// LOG(DEBUG) << "Disturbances from animal model ";
	// for (auto& d : dist){
	// 	LOG(DEBUG) << "Disturbance type: "<< d.first << " with magnitude "<< d.second;
	// }
	
	if (m_FGdist_ptr->distResponse.contains(LifeStage::Immature)){
		// LOG(DEBUG) << "Disturbances known for immatures of PFG "<< getName()<< ": " << m_FGdist_ptr->distResponse.at(LifeStage::Immature).size();
		std::map<std::string, const Dist> temp = m_FGdist_ptr->distResponse.at(LifeStage::Immature);
		// LOG(DEBUG) << "Disturbances known for immatures: " << temp.size();
		for (auto& p : temp){
			// LOG(DEBUG) << "Disturbance type: "<< p.first << " with magnitude "<< p.second.propKilled;
			if (dist.contains(p.first) ){
				reduceCohort(0, (m_FGparams_ptr->M-1), (1- p.second.propKilled * dist.at(p.first)));
				//reduce seed pool by seedsKilled
				//ignore resprouting
				// LOG(DEBUG) << "Disturbed.";
			}
			// else {LOG(DEBUG) << "no Disturbance found";}
		}
	}
	// else {LOG(DEBUG) << "No disturbances known (immatures) for PFG " << getName();}
	
	//same for matures
	if (m_FGdist_ptr->distResponse.contains(LifeStage::Mature)){
		// LOG(DEBUG) << "Disturbances known for matures of PFG "<< getName()<< ": " << m_FGdist_ptr->distResponse.at(LifeStage::Mature).size();
		std::map<std::string, const Dist> temp = m_FGdist_ptr->distResponse.at(LifeStage::Mature);
		for (auto& p : temp){
			// LOG(DEBUG) << "Disturbance type: "<< p.first << " with magnitude "<< p.second.propKilled;
			if (dist.contains(p.first)){
				reduceCohort(m_FGparams_ptr->M, m_FGparams_ptr->L, (1- p.second.propKilled * dist.at(p.first)));
				// LOG(DEBUG) << "Disturbed.";
			}
			// else {LOG(DEBUG) << "no Disturbance found";}
		}
	}
	// else {LOG(DEBUG) << "No disturbances known (Matures) for PFG " << getName();}
}

/*----------------------------------------------------------------------------*/
//getters

int FuncGroup::getCount(int Age0, int Age1) const {
	int MaturityAge = m_FGparams_ptr->M;
	assert(Age0 >=0 && Age1 >= 0);
	if (Age0 > Age1) std::swap(Age0, Age1);
	if (m_cohorts.size() == 0){return(0);}

	/* initialize count to 0 */
	int CsizeTot = 0;
	for (int i=0; i<m_cohorts.size(); i++){
		int Ay = m_cohorts.at(i).Ay;
		int Ao = m_cohorts.at(i).Ao;


		if (Age0 > Ao || Age1 < Ay){ continue; } /* check if cohort is concerned */
		else{
			int Csize = m_cohorts.at(i).CSize;

			if (Ao >= MaturityAge && Age1>= MaturityAge){ // some matures in this cohort
				int matLegPart = std::min(Ao, Age1) - std::max(MaturityAge, std::max(Ay, Age0) ) + 1;
				CsizeTot += matLegPart * Csize;
			}

			if (Ay < MaturityAge && Age0 < MaturityAge){ // some immatures in this cohort
				int immLegPart = std::min(MaturityAge -1, std::min(Ao, Age1) ) - std::max(Ay,Age0) + 1;
				CsizeTot += (immLegPart * Csize * m_FGparams_ptr->ImmSize);
			}

		}
	} // end of loop on cohorts
	return CsizeTot;
} 

int FuncGroup::getCount() const{
	return getCount( 0, m_FGparams_ptr->L );
}


int FuncGroup::getStratAbund(int stratum) const{
	assert(stratum >= 0);
	if (stratum > m_FGparams_ptr->MaxStratum ) return 0;

	const std::vector<int>& Strata {m_FGparams_ptr->Strata};
	int youngest = Strata[stratum];
	int oldest;
	if (stratum == m_FGparams_ptr->MaxStratum) oldest = m_FGparams_ptr->L; else oldest = Strata[stratum+1] - 1;
	return static_cast<int> (getCount(youngest, oldest) * m_FGparams_ptr->LightShadeFactor);
}

int FuncGroup::getFecund() const{
	int matAbund =0;
	/* get mature Abundance = any individual between maturation and max life span*/
	if (m_FGparams_ptr->M < m_FGparams_ptr->L){
	  matAbund = getCount(m_FGparams_ptr->M, m_FGparams_ptr->L);
	 }
//	 logg.debug("matAbund = ", matAbund, "  m_abundLimit = ", m_abundLimit, "  m_FGparams->getPotentialFecund() = ", m_FGparams->getPotentialFecund());
	return std::min(matAbund, m_abundLimit) * m_FGparams_ptr->potentialFecundity;
}

std::string FuncGroup::getName() const{return(m_FGparams_ptr->name);}


void FuncGroup::show() const{
	CLOG(DEBUG, "PLANTS") << "FunctGroup :" << "\nFG = " << m_FGparams_ptr->name;
	for (unsigned i =0; i<m_cohorts.size(); i++){
		CLOG(DEBUG, "PLANTS") << "SIZE: " << m_cohorts[i].CSize << "  AY: " << m_cohorts[i].Ay << " AO: " << m_cohorts[i].Ao;
	}
	for (unsigned i=0; i<m_pools.size();i++){
		m_pools[i].show();
	}
}


bool FuncGroup::checkSoilClass(std::string_view soilClass) const{
	if (m_FGparams_ptr->soilTol.size()==0){LOG(FATAL) << "This PFG has no soil tolerance";}

	for (int i = 0; i < m_FGparams_ptr->soilTol.size(); i++) {
		if (m_FGparams_ptr->soilTol[i] == soilClass) return true;
	}
	return false;
}

bool FuncGroup::checkSoilDepth(int soilDepth) const{
	assert (soilDepth >= 0);
	if (soilDepth < m_FGparams_ptr->DepthReq) return false;
	else return true;
}

void FuncGroup::remove(){
	removeCohort(0, m_FGparams_ptr->L);
}

//==============================================================================
//private

/*----------------------------------------------------------------------------*/
//functions to perform the main tasks

void FuncGroup::AgePool1(const PoolType& pt){
	assert(pt < PoolType::PTcount); 
	
	m_pools[pt].AgePool1(m_FGparams_ptr->PoolL[pt]);
}

void FuncGroup::AgePool1(){
	for (int pt=0; pt<PTcount; pt++){
		this->AgePool1((PoolType) pt);
	}
}

int FuncGroup::getMatTime(bool affected_by_env){
	assert (!affected_by_env); //function is a relic from FATE and I dont like it.
	
	if (affected_by_env) {return m_FGparams_ptr->L;} 	// affected by environment
	else {return m_FGparams_ptr->M;} 					// not affected by environment
}



int FuncGroup::doRecruitment(int seeds, const GSP_PLANTS* const gsp, const Resource& R0, const bool affected_by_env){
	assert(seeds >= 0);
	if (affected_by_env) return 0;
	// do recruitment only if abundance in stratum 0 is <  max abund * (1 + ImmSize)
	const std::vector<int> abunds{gsp->maxAbundLow,gsp->maxAbundMedium, gsp->maxAbundHigh};
	if(getStratAbund(0) >= ((1+ m_FGparams_ptr->ImmSize) * abunds[static_cast<int>(m_FGparams_ptr->MaxA)-1])) return 0;

	else {
		addCohort( static_cast<int>(seeds), 0, 0);
		return seeds;
	}
}

/*----------------------------------------------------------------------------*/
	//functions to manipulate cohorts


void FuncGroup::addCohort(const int& cSize, int ay, int ao){
	if (ay > ao) std::swap(ay, ao);
	assert(cSize >= 0 && ay >= 0 && ao >= 0	);
		//each cohort has a minimum age (ay) and a maximum age (ao);
		//if the new cohort is younger than the minimum age of any cohort, it is added at the beginning of the vector
		//if it is older than the maximum age of the any cohort, it is added at the end of the vector
		//if it is within the range of the cohorts, then the cohort is disassembled into individual ages, 
		//the new cohort is added, and the thing is put back together again
	if (cSize == 0){ return;}
	int tempCSize = cSize;

	int minAge = m_cohorts.front().Ay;
	int maxAge = m_cohorts.back().Ao;

	if (ao < minAge) m_cohorts.insert(m_cohorts.begin(), Cohort{tempCSize, ay, ao}); //new cohort is younger
	else if (ay > maxAge) m_cohorts.push_back(Cohort{tempCSize, ay, ao}); //new cohort is older
	else { //disassembly required
		std::vector<int> tempCohortList(std::max(ao, maxAge)+1, 0);
		for (auto it = m_cohorts.begin(); it != m_cohorts.end(); it++){
			for (int i = it->Ay; i <= it->Ao; i++){
				tempCohortList[i] += it->CSize; //+= in case there are overlapping cohorts
			}
		}
		//add the new cohort to the vector
		for (int i = ay; i <= ao; i++){
			tempCohortList[i] += tempCSize;
		}
		//merge the vector into a new cohort list
		m_cohorts.clear();
		for (int i = 0; i < tempCohortList.size(); i++){
			if (tempCohortList[i] > 0) m_cohorts.push_back(Cohort{tempCohortList[i], i, i});
		}
		pickupCohorts();
	}
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void FuncGroup::removeCohort(int ay, int ao){
	assert(ay >= 0 && ao >= 0);
	if (ay > ao) std::swap(ay, ao);

	int minAge = m_cohorts.front().Ay;
	int maxAge = m_cohorts.back().Ao;

	//distinguish three cases:
	//1: ay and ao are outside the cohorts range: delete nothing
	//2: all cohorts falls fully inside ay, ao: delete the whole cohort
	//3: cohorts are partially within ay, ao: ignore the cohorts that are outside the range, 
	//	cut the cohorts that are partially in the range

	if (ay > maxAge || ao < minAge)  return; //case 1
	else if (ay <= minAge && ao >= maxAge){ ///case 2
		m_cohorts.clear();
		return;
	} else {  //case 3
		for (auto it = m_cohorts.begin(); it != m_cohorts.end();){
			if (it->Ao < ay || it->Ay > ao){ //ignore cohorts outside the range
				++it;
			} else if (it->Ao <= ao && it->Ay >= ay){ //delete cohorts fully within the range
				it = m_cohorts.erase(it);
			} else {   //cut cohorts partially within the range
				if (it->Ay < ay){
					it->Ao = ay - 1;
				}
				if (it->Ao > ao){
					it->Ao = ao;
				}
			}
		}
	}
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void FuncGroup::reduceCohort(int ay, int ao, const double& multiplyBy){
	assert(ay >= 0 && ao >= 0 && multiplyBy >= 0 && multiplyBy <= 1);
	if (m_cohorts.size() == 0){ return;}

	//need to split the cohorts around the ay/ao values, so that cohorts get differentially affected
	for (unsigned co=0; co<m_cohorts.size(); co++){
		if (m_cohorts[co].Ay > ao){ // no more concerned cohort : ao < getAy
			break;
		}
		if (m_cohorts[co].Ao < ay){ // makes the loop jump until we get to the first concerned cohort
			continue;
		}
		if (m_cohorts[co].Ay < ay){ // INSIDE THE COHORT : ay > getAy : split the cohort and call recursively
			this->splitCohort(co, ay-1);
			continue;
		}
		if (m_cohorts[co].Ao > ao){ // INSIDE THE COHORT : ao < getAo : split the cohort and call recursively
			this->splitCohort(co, ao);
		}

		unsigned newSize = (unsigned) ( m_cohorts[co].CSize * multiplyBy );
		if (newSize > 0){ /* 2. REDUCE THIS cohort ABUNDANCE + CONTINUE THE LOOP */
			m_cohorts[co].CSize = newSize;
		} else{ /* 3. REMOVE THE COHORT */
			this->removeCohort(m_cohorts[co].Ay, m_cohorts[co].Ao);
			co--;
		}
	}
	/* pick up cohorts (optional) */
	this->pickupCohorts();
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void FuncGroup::splitCohort(const int& i, const int& age){
	assert(i >= 0 && age >= 0);
	std::vector< Cohort >::iterator it = m_cohorts.begin() + i + 1;
	Cohort newCohort{m_cohorts[i].CSize, age + 1, m_cohorts[i].Ao}; //copy but age +1
	m_cohorts[i].Ao = age; //original
	m_cohorts.insert(it, newCohort);
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void FuncGroup::pickupCohorts(){
	if (m_cohorts.size()<= 1){ return; }
	for (unsigned co=0; co<m_cohorts.size()-1; co++){
		//check if successive cohorts have the same abundance and are juxtaposed (ao + 1 = ay)
		if ((m_cohorts[co].CSize == m_cohorts[co+1].CSize && (m_cohorts[co].Ao + 1) == m_cohorts[co+1].Ay)){
				m_cohorts[co].Ao  = m_cohorts[co+1].Ao; 
				m_cohorts.erase(m_cohorts.begin() + co + 1 ); 
				pickupCohorts();
		}
	}
}
