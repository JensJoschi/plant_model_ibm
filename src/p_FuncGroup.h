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

 
/*!
 * \file p_FuncGroup.h
 * \brief Plant Functional Groups (seeds and cohorts)
 * \author Damien Georges, Jens Joschinski
 * \version 2.0
 * \date 2023/03/28
 */

#ifndef FUNCGROUP_H
#define FUNCGROUP_H

#include "g_GSP_PLANTS.h"
#include "p_PFG.h"
#include "p_PropPool.h"
#include "g_FGUtils.h"
#include "p_Cohort.h"
/** @cond */
#include <string>
#include <vector>
#include <string_view>
/** @endcond */

class PFG;
class PFGDisturbances;

/*!
 * \class FuncGroup
 * \brief Plant Functional Group Class
 * \details
 * A Functional Group is defined by a vector of two Propagules Pools (active
 * and dormant) and a list of cohorts containing the
 * abundances of all individuals of specific ages.
 * The FuncGroup is the object that is modified by demographic processes
 * (recruitment, survival, aging). 
 * It is defined for one particular group of plants (FG) having the same life
 * attributes, and may be brought together with other Functional Groups
 * to form a Community.
 * \note seeds that could not germinate in a year are put back into seed pool
 * Biologically this makes no sense (they should die); it would be better if a
 * certain proportion of seeds get put into the seed pool every year, and the rest attempts to germinate
 * (requires the two variables germinationRate and dormancyMortality). To implement, PropPools needs to be rewritten
 * (see comment in p_PropPool.h)
 */
class FuncGroup{

//==============================================================================
	public:
	FuncGroup();
	/**
	 * \brief construct a FuncGroup with Community build instructions
	 * \param pfg pointer to a PFG object containing PFG definitions
	 * (life span, fecundity, etc.)
	 * \param pfgdisturbance pointer to a PFGDisturbances object containing PFG disturbance definitions
	 * \param cohortSize vector of size 4, containing abundance, min and max year 
	 * of each cohort, and number of cohorts (keep number at 1!)
	 * \param poolSize seeds in the active seed pool
	 * \param low maximum abundance that a "low" Abund PFG can reach
	 * \param med maximum abundance that a "med" Abund PFG can reach
	 * \param high maximum abundance that a "high" Abund PFG can reach
	 * \note the fourth number in the cohortSize vector is silently ignored. will be removed entirley
	 */
	FuncGroup(const PFG* pfg, const PFGDisturbances* pfgdisturbance, const std::vector<int>& cohortSize, const int poolSize, 
	const int low, const int med, const int high);

		FuncGroup(const PFG* pfg, const PFGDisturbances* pfgdisturbance, const std::vector<int>& cohortSize, const int poolSize, 
	const int low, const int med, const int high, int n); //added for debugging of unit tests. To delete.

/*----------------------------------------------------------------------------*/
//main functions

	/*!
	 * \brief calculate seed germination and recruitment
	 \details Sequence of events:
	 * 1. Age the seed pools
	 * 2. Determine the size of the propagule rain (local and external)
	 * 3. Empty the active seed pool
	 * 4. Actually recruit a cohort only if germinants are able to withstand the
	 *    environment in stratum 0.
	 * 5. Germination is a function of the degree of enforced dormancy and of the 
	 *    size of the pool of available seeds 
	 * 6. Now place the propagules in the pool (this ensures that shortlived
	 *    propagules at least survive through disturbances).
	 * 
	 * \note JJ: Instead of emptying the active seed pool, the pool should probably be deleted and 
	 * replaced by a new one in step 6.
	 * \note JJ: should proobably be two functions but i can't be arsed
	 * 
	 * \param gsp plant simulation parameters
	 * \param seeds_in current seed rain (local dispersal and regional model)
	 * \param R0 light resources in lowest stratum (ground)
	 * \param affected_by_env is the PFG negatively affected by the environment (reduces recruitment to zero)?
	 */
	void germinateAndRecruit(const GSP_PLANTS* const gsp, const int seeds_in, const Resource& R0, const bool affected_by_env );
	
	/*!
	 *	\brief Age all individuals
	 *  \details 
	 *	This functions increments the age of all individuals of all cohorts by 1. 
	 *	If the older cohort reaches the maximum age of the
	 *  functional group, this last cohort is removed from the list.
	 *
	 *	\param maxAge : maximum age that can be reached by an individual. If -1,
	 * it corresponds to the functional group life span.
	 *  \note this function is bogus. see comment in the code.
	 */
	void age(int maxAge = -1);

	/*!
	 *	\brief Check PFG survival under light conditions
	 * \details 
	 * This function compares the light resources with the tolerance of the 
	 * functional group. PFG biomass that is not tolerant to the current light
	 * conditions will be removed. 
	 * cohorts may grow over several layers (e.g. containing 1-5 yr old plants),
	 * hence the function may need to split cohorts.
	 * \param light current light resources 
	 * \note JJ: growing across layers requires rather complex operations
	 * (cohort splitting and joining). Resizing a vector or inserting something 
	 * into the middle of a vector is costly, and maintaining/debugging such code is difficult. 
	 * I doubt that the potential to save some memory warrants such computationally 
	 * complex operations (increases processing time).
	 */
	void die(const std::vector<Resource>& light);


	/**
	 * \brief disabled
	 * 
	 * \param distName 
	 * \param distMagnitude 
	 */
	void beDisturbed(const std::map<std::string, double>& disturbances);

	/** 
	 * \brief remove all cohorts*/
	void remove();

/*----------------------------------------------------------------------------*/
//getters

	/*!
	 *	\brief Calculate abundance of individuals between two ages
	 *  \details 
	 *	This function returns the abundance of individuals
	 * between two given ages (which may include multiple cohorts).
	 *  It considers relative size of mature and immature
	 * individuals (abundMature + abundImmature * ImmSize).
	 *
	 *	\param Age0 : age of the youngest individuals
	 *	\param Age1 : age of the oldest individuals
	 * \return : the number of individuals between two ages
	 */
	int getCount(int Age0, int Age1) const;


	/*!
	 *	\brief Calculate abundance of the complete PFG
	 *  \details 
	 *	This function returns the abundance of individuals. It
	 * considers relative size of mature and immature individuals (abundMature
	 * + abundImmature * ImmSize).
	 *
	 * \return : the total abundance
	 */
	int getCount() const;

		/**
	 * \brief calculate the abundance in a stratum
	 * 
	 * \param stratum stratum on which to perform the calculation
	 * @return This function returns the abundance of individuals. It
	 * DOES NOT consider the relative size of mature and immature individuals 
	 * (abundMature + abundImmature * ImmSize).
	 */
	int getStratAbund(int stratum) const;

	/*!
	 *	\brief Calculate fecundity : amount of seeds that will be produced this
	 * year
	 * \details
	 * The fecundity is represented as a theoretical number of seeds. Each
	 * PFG has a potential number of seeds to be produced each year (Potential
	 * Fecund). This number is weighted (understand that it can only be
	 * decreased) by an evaluation of the importance of mature individuals for
	 * the current year compared to its maximal potential through time.
	 * This number is represented by the total abundance of mature individuals
	 * (between Maturity age and Lifespan) ; divided by the maximum abundance of
	 * the PFG, divided by its theoretical number of mature years (Lifespan -
	 * Maturity age).
	 *
	 * \return : number of seeds that will be produced this year for this PFG
	 */
	int getFecund() const;

	std::string getName() const;

	void show() const;

	/**
	 * \brief check soil tolerance.
	 * \details this function checks whether the soil class matches the
	 * PFG requirements. 
	 * 
	 * \param soilClass 
	 * @return true if soil class matches
	 * @return false  if soil class does not match
	 */
	bool checkSoilClass(std::string_view soilClass) const;

	/**
	 * \brief check soil tolerance.
	 * \details this function checks whether the soil depth matches the
	 * PFG requirements. 
	 * 
	 * \param soildepth
	 * @return true if soil depth is sufficient
	 * @return false if soil depth is insufficient
	 */
	bool checkSoilDepth(int depth) const;

//==============================================================================
	private:

/*----------------------------------------------------------------------------*/
//functions to perform the main tasks

	/*!
	 *	\brief Age seeds in a pool and calculate new abundance
	 *  \details
	 * Seed mortality rate is considered to follow a linear relationship as a
	 * function of seed life :
	 * size (n+1) = size (n) - size(n) * (1 / (pl + 1))
	 * \warning it seems that this function is not being used
	 *
	 *	\param pt : selected pool (DormantP or ActiveP)
	 */
	void AgePool1(const PoolType& pt);
	

	/*!
	 *	\brief Age seeds in all pools and calculate new abundances
	 *  \details
	 * Seed mortality rate is considered to follow a linear relationship as a
	 * function of seed life :
	 * size (n+1) = size (n) - size(n) * (1 / (pl + 1))
	 * \warning it seems that this function is not being used
	 */
	void AgePool1();

	//little helper for die
	int getMatTime(bool affected_by_env);


	/*!
	 *	\brief Recruitment of seeds
	 *  \details the seed input will be checked for survival and added to the active pool. 
	 * If successful, a reduced seed set will be returned, and a new cohort will 
	 * be added to the list
	 * \param seeds : number of seeds to recruit
	 * \param gsp : plant simulation parameters
	 * \param R0 : light resource at the ground level
	 * \param affected_by_env : true if the PFG is negatively affected by the environment
	 * \return : number of seeds that remain
	 * \note : the active pool is updated, and a new cohort is added
	 */
	int doRecruitment(int seeds, const GSP_PLANTS* const gsp, const Resource& R0, const bool affected_by_env);



/*----------------------------------------------------------------------------*/
	//functions to manipulate cohorts

	/*!
	 *	\brief Insert a cohort of individuals into the cohort list
	 *  \details
	 *	This routine inserts a cohort composed of (ay - ao) generations of cSize
	 * individuals into the list of cohorts.
	 * A new cohort can be created, an older one can be split or modified, to
	 * keep a well sorted  list.
	 *
	 *	\param cSize : abundance of each generation of the new cohort
	 *	\param ay : age of the youngest individuals of the new cohort
	 *	\param ao : age of the oldest individuals of the new cohort
	 */
	void addCohort(const int& cSize, int ay, int ao);


	/*!
	 *	\brief Remove a cohort of individuals (one or several generations) from
	 * a cohort list
	 *  \details
	 *	This routine removes all cohorts whose age is between ay and ao.
	 * A whole cohort can be removed, an older one can be split or modified, to
	 * keep a well sorted Legion list.
	 *
	 *	\param ay : age of the youngest individuals to be removed
	 *	\param ao : age of the oldest individuals to be removed
	 */
	void removeCohort(int ay, int ao);


	/*!
	 *	\brief Reduce the abundance of a cohort of individuals (one or several
	 * generations) from a cohort list
	 *  \details
	 *	This routine reduces the size of all cohorts whose age is between ay and
	 * ao. The reduction factor is given by reducFact.
	 * A whole cohort can be removed, an older one can be split or modified, to
	 * keep a well sorted Legion list.
	 *
	 *	\param ay : age of the youngest individuals whose abundance should be
	 * reduced
	 *	\param ao : age of the oldest individuals whose abundance should be
	 * reduced
	 * \param reducFact : the cohort abundance reduction factor
	 * \note JJ: it looks like this function can alos be used to increase a cohort,
	 * by choosing a "reducFact" > 1.0. This is not documented in the code.
	 * Just remove the assertion in the code.
	 */
	void reduceCohort(int ay, int ao, const double& reducFact);

	/*!
	 *	\brief Split a cohort of individuals in 2 parts
	 *  \details
	 *	This function split a given cohort in 2 parts to let other functions
	 * manipulate them correctly (increase or decrease abundance of specific
	 * generations, remove specific generations, etc).
	 *
	 *	\param i : the index within the cohort list to choose cohort to split
	 *	\param age : the breaking age used to split the selected cohort
	 */
	void splitCohort(const int& i, const int& age);

	/*!
	 *	\brief Pick up all cohorts of individuals if possible
	 *  \details
	 *	This routine picks up adjacent cohorts (successive generations) having
	 * the same size.
	 * The aim is to prevent from memory wasting.
	 */
	void pickupCohorts();

/*----------------------------------------------------------------------------*/

	std::vector<PropPool> m_pools; /*!< Propagules pool array (active & dormant seed pool) */
	std::vector<Cohort> m_cohorts; 	/*!< cohort list */
	const PFG* m_FGparams_ptr; 	/*!< FG parameters */
	const PFGDisturbances* m_FGdist_ptr; /*!< FG disturbance parameters */
	const int m_abundLimit; /*!maximum abundance that this pfg can reach (determined by PFG:maxAbund class and GSP)*/
};
#endif //FUNCGROUP
