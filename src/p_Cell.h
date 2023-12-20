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
 // JJ: added shading, complete restructuring and cleanup; calculate light passing through neighbour cells
 // ---------------------------------------------------------------------------

 
/*! 
 * \file p_Cell.h
 * \brief Succession (demographic model) class
 * \author Damien Georges, Maya Gueguen, Jens Joschinski
 * \date 2023
 */

#ifndef CELL_H
#define CELL_H

#include "p_FuncGroup.h"
#include "g_GSP_PLANTS.h"
#include "p_PFGDefs.h"
#include "g_FGUtils.h"
/** @cond */
#include <vector>
#include <string>
#include <string_view>
#include <map>
/** @endcond */




//===============================================
// Future version
//all same except do Succession:
//each FG: germinate(disturbance[i])  //seeds from dormant may be activated, seeds may be killed by age or disturbance

//each FG: recruit(envSuit[i]) //plants die if light or soil is not sufficient  

//community: disturb(disturbance[i]):
// while disturbances > 0: disturbances = randomPFG.disturb(disturbances) //reduce PFG by disturbance if suitable; also reduce disturbance a bit; 
                                                                        //reduced part may be placed in map "resprouting" if applicable

//light_conditions = updateLight() //competition for light is only based on unaffected plants
// each FG: die (shade)
//each FG: age()
//each FG: add resprout(shade)
//each FG: makeSeeds()


//===============================================
 /*!
 * \class Cell
 * \brief Succession (demographic model) class
 * \details
 * This class contains the demographic model of the plant community.
 * It contains the plant community, available light ressources and the degree of 
 * shading of a cell, as well as the functions to call the succession routines 
 * (including habitat suitability, animal disturbance and competition for light).
 * \note receives the full PFGDefs, should get only a reduced regional model set.
*/
class Cell{

//==============================================================================
	public:

	/*!
	 *	\brief Constructor with community build instructions
	 *	calls the FuncGroup constructor with pfgs, commsize and poolsize
	 *
	 * \param gsp_plants : pointer to GSP class object containing plant simulation
	 * parameters
	 * \param pfgs : pointer to PFGDefs class object containing PFG definitions
	 * (life span, fecundity, etc.)
	 * \param shade percentage of shading in this cell
	 * \param depth soil depth in this cell
	 * \param commSize : vector of size 4, containing abundance, min and max year 
	 * of each cohort, and the number of cohorts (same for all PFGs)
	 * \param poolSize : seeds in the active seed pool
	 */
	Cell(const GSP_PLANTS* const gsp_plants, const PFGDefs* pfgs, const double shade, const int depth, const std::vector<int>& commSize, const int poolSize);

/*----------------------------------------------------------------------------*/
	/*!
	*  \brief Create habitat unsuitability based on soil class
	*  \details This function compares available soil class and soil depth with a PFG's 
	*   requirements, and derives a "fitness value" from it. Fitness is compared against 
	*   a random "environmental severity" number. If smaller, the environment is unsuitable for the PFG
	*   Function should be called with a soil class if gsp.doesSoilClass, otherwise soilclass can be omitted
	*	\param killExisting if true, kills all plants of the PFG which are not suitable
	*	\param soil soil class to compare against
	*   \note  Currently m_envsuitMap is a bool, so the environmental severity has no effect. 
		Could be changed to a probability
	*/
	void CreateHabSuit(bool killExisting, std::string_view soil = "");
	
	/**
	* \brief create  disturbance map
	* \details this function creates a disturbance, simply by passing on the disturbances from PlantModel.
	* \param animals biomass
	\note  Problem: the animal disturbance 
		// values should be between 0 and 1, but is a biomass in kg. there is no 
		// conversion used yet. 
		// as a mouse weighs only 0.2 kg max, it will currently kill 0.2 * 100% 
		// of the plant abundances, so it effectively only eats 20% of available
		// plant material. Right now every animal disturbs plants, so we can see something
	*/
	void CreateDisturbances(const std::map<std::string, double>& dists);


	/*!
	*	\brief Do demographic FATE model
	* 	\details The succession model is based on the following steps:
	* 1. Determine the environmental conditions and kill established plants which 
		cannot tolerate current light availability
	* 2. Age established plants
	* 3. Recalculate the environmental conditions for recruitment
	* 4. calculate seed germination and recruitment
	* 5. calculate fecundity

	* In contrast to RFate, the current light availability is influenced by the 
	* amount of shading in a cell, and by the combination of sun angle and neighbour's plant density
	* \param seeds_in seed rain from the dispersal model and the regional model
	* \param plant density of neighbouring cell, vector of size noStrata (not of size noPFG!)
	* \return map of seeds produced by each PFG
 	*/
 	std::map<std::string, int> DoSuccession(const std::map<std::string, int>& seeds_in);
	std::map<std::string, int> DoSuccession(const std::map<std::string, int>& seeds_in, const std::vector<int>& neighbour);
/*----------------------------------------------------------------------------*/


	/*! 
	* \brief gets total abundance of a PFG
	*/
	int getPFGabund(const int pfg);


//==============================================================================
	private:

	std::vector<FuncGroup> m_funcGroups; 		/*!< state of each PFG */
	const GSP_PLANTS* const m_gsp_ptr;	/*!< pointer to plant simulation parameters*/

	std::vector<bool> m_EnvUnsuitability; /*!< flags if habitat is unsuitable for a functional group */
	double m_EnvSuitRefVal; 			/*!< reference value for environmental suitability index */
	const int m_depth;				/*!<Depth of the cell*/

	std::vector<Resource> m_LightR; 		/*!< Light Resources state in each stratum */
	const double m_shading;  		/*!<How much is light resource reduced by shading*/

	std::map<std::string, double> m_disturbances; /*!< degree of disturbance by multiple factors (animals, fire, etc.)*/

	/*!
	*  \brief update environmental suitability index 
	*	\details draw random number between 0 and 1 (uniform distribution). 
	*	The number is an index of how bad a year is.
	*	\note This whole function is bad, because rng gets reseeded every time this function is called
	* 	but currently envsuit is bool anyway, so envsuitrefval has no effect
	*/
	double UpdateEnvSuitRefVal();


	/*!
	 *	\brief Calculate light values based on FG abundances
	 *  \details 
	 * Light resources are calculated per stratum (starting from the top and any
	 * remaing light passing down), according to the PFG abundances, and according
	 * to the thresholds defined in the GSP object :
	 * LIGHT_THRESH_MEDIUM and LIGHT_THRESH_LOW.
	 * If the cumulated abundance in one stratum is superior to one of these
	 * thresholds, the light resources in the stratum below will decrease.
	 * It is impossible to have more light in one stratum than in the upper
	 * stratum. In contrast to Fate:
	 * - All light resources are corrected for the degree of shading in a cell.
	 * - Light passes partially through the neighbour's canopy, depending on the
	 * sun angle, so the neighrbous density influences this cell's light availability.
	 * 	\param abundance vector of size noStrata, containing the plant density of the neighbouring cell
	 * 	\param angle angle at which light falls on the cell; used to calculate how much light passes through the neighbour
	 */
	public: // made these public for testing
	void updateLight(const std::vector<int>& abundance, const int angle);

	void updateLight(); //for use without a neighbour/angle of 90°

	/**
	 * \brief calculate the amount of light that passes through the neighbour's canopy
	 * \details The angled light ray creates a right triangle with the ground and the 
	 * height of the stratum. The light entering the ground is accordingly stratum height/tan(angle)
	 * 
	 * \param lightAbove amount of light getting through the stratum above
	 * \param angle  angle at which light falls on the cell
	 * \param lightNeighbour amount of light getting through the neighbour
	 * \param height of the stratum
	 * @return int light that is left
	 */
	int calculateLight(const int lightAbove, const int angle, const int lightNeighbour, const int height);

	/**
	 * \brief calculate the abundance of all PFGs in each stratum
	 * @return vector<int> abundance
	 */
	std::vector<int> calcAbundances ();


};
#endif //CELL