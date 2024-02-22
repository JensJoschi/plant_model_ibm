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
 // JJ: cleanup and removal of unused code (EPM)
 // ----------------------------------------------------------------------------


/*!
 * \file p_PropPool.h
 * \brief Propagules Pool (seeds) Class
 */

#ifndef PROPOOL_H
#define PROPOOL_H

/*!
 * \class PropPool
 * \brief Basal Structure to store PFG propagules
 * \details 
 * It is defined by its size (seeds abundance), the age of the last seeds put in
 * the pool, and a boolean defining if seeds are declining through time or not
 * (meaning some of them will die at the next time step). Each plant functional 
 * group (defined by the FuncGroup class) has two pools of seeds, one active and 
 * one dormant. In most cases, only the active pool will be used.
 * \note JJ:
 * The number of seeds in the propagule pool (size) generally declines over time
 * (seed mortality), unless mature plants produce new seeds and the new seeds get
 * put in the pool. The original FATE-HD publication and the user guide do not document
 * seed mortality, and also mention no rules regarding addition of new seeds (e.g. should
 * it be density-dependent?). Currently the PutSeedInPool function simply replaces the
 * existing pool with the new abundance of seeds, but only if the new pool is larger
 * than the old one. 
 * The age of the last seeds put in the pool is never actually used and may be erased. 
 * The boolean is also probably unneccessary and could be removed. The seed pool life span, 
 * which determines how fast the seed pool declines, on the other hand, should be a 
 * property of the seed pool. The "life span" is close to the half-life of the seed pool,
 * but not accurate (the actual Life span would be infinite but with lim->0)
 */
class PropPool{

//==============================================================================
	public:

	/*!
	 *	\brief Full constructor
	 *
	 *	\param size : size of pool (abundance, percentage of potential fecundity)
	 *	\param declining : is the pool declining ?
	 *	\param dTime : age of the youngest seeds of the pool
	 */
	PropPool(int size, bool declining, int dTime);


/*----------------------------------------------------------------------------*/

	/*!
	 *	\brief Add seeds in the pool
	 *  \details
	 *	This function compares the new abundance of seeds (Inp) to the abundance
	 * of seeds already present in the pool.
	 * If the new abundance is greater, the pool becomes :
	 * Pool(size = Inp, declining = false, dTime = 0),
	 * otherwise, the propagules pool is not modified.
	 *
	 *	\param Inp : abundance of seeds to be added
	 */
	void PutSeedInPool(int Inp);

	/*!
	 *	\brief Empty the seeds pool
	 *  \details
	 *	All parameters of the propagules pool are set to 0, none or FALSE.
	 	\note JJ: one could write a destructor for this instead
	 */
	void EmptyPool();

	/*!
	 *	\brief Age seeds in the pool and calculate new abundance
	 *  \details 
	 * Seed mortality rate is considered to follow a linear relationship as a
	 * function of seed life :
	 * size (n+1) = size (n) - size(n) * (1 / (pl + 1))
	 *	\param pl : seeds life span
	   \note JJ: seed life span is a constant (PFG property), so the mortality rate
	 is also constant. A constant mortality rate leads to exponential decline.
	 */
	void AgePool1(int pl);

/*----------------------------------------------------------------------------*/

	int getSize() const;
	void show() const;

//==============================================================================
	private:
	int m_Size; /*!< Abundance of seeds of the pool, % of Potential Fecundity */
	bool m_Declining; /*!< Is the pool declining ? */
	int m_DTime; /*!< Age of the youngest seeds of the pool */


	//this class should have:
	//int active; 		//current seed mass
	//int inactive;		//current seed mass that is dormant (not germinating)
	//int capacity;		//max seed mass
	//PropPool(int active, int inactive, int capacity)
	//capacity can never be changed after construction.

	//void DoMortality(double m = 1.0, bool dormant = false); //kill m% of seeds. m is given by PFG attribute
	//void inactivate (double r =1.0) //put n% seeds from active to dormant stage
	//void activate (double r= 1.0) //the rate should also be a pfg attribute

	//void increase(int n, bool dormant = false) //increase active or dormant pool by n
	//void decrease(int n, bool dormant = false)
};

#endif // PROPOOL
