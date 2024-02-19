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
 // RFate team: provide original code
 // JJ: split SimulMap into a PlantModel class and a EcolopesLand class (different project); 
 //     rewrote habitat suitability and disturbance functions
 //     functions to exchange data with other models
 // VMVC: functions to exchange data with other models
 // ----------------------------------------------------------------------------

/*!
 * \file p_plantmodel.h
 * \brief main class of the plant model
 * \details this file contains the PlantModel class. It contains the dispersal 
 * and succession models, seed, habitat
 * suitability and (animal) disturbance maps, and functions to create the habitat suitability / disturbance maps, and to
 * invoke succession and dispersal
 * \author Jens Joschinski
 * \version 1.0
 */

#ifndef PLANTMODEL_H
#define PLANTMODEL_H

#include "Landscape.h"
#include "Cell.h"
#include "g_Inputs.h"
#include "GSP_PLANTS.h"
#include "Data_PLANTS.h"
typedef Inputs<GSP_PLANTS, Data_PLANTS> Inputs_P;

/** @cond */
#include <vector>
#include <string>
#include <map>
/** @endcond */
#include "easylogging++.h"


/*!
 * \class PlantModel
 * \brief core of the Plant model
 * \details 
 * This class contains
 * 1) the dispersal and succession models
 * 2) seed maps
 * 3) functions to query data fom the other models and to invoke succession and dispersal
 * \note during construction the model will place 10 seeds of each PFG in each cell
 * 
 * \warning: seeds from regional model may be inserted twice (dispersal and inside Cell::germinateandRecruit). to check
 */
class PlantModel{

//==============================================================================
	public:
	/**
	 * \brief constructor

	 */
	explicit PlantModel(const std::string& inputFile, const std::string& logConfig = "", bool FixRNG = false);

	/**
	 * \brief Create stable starting community
	 * \details
	 * Wrapper function. Runs the pland model for "years" timesteps to initiate 
	 * a community, doing only succession and dispersal     
	  * Also loads disturbances and soil classes from file, if activated. 
	 * \param years number of time steps to run
	 */
	void initialize(int years);

	/**
	 * \brief Perform one plant time step
	 * \details Wrapper function. Calls succession and dispersal functions
	 */
	void TPlusOne_JJ();

	/**
	 * \brief create inputs for plant model
	 * \details Wrapper function to create habitat suitability and disturbance maps
	 * Conceptually converts a Landscape<soils> and a landscape <effectsize> into a landscape<habitat suitability> and a landscape<disturbance>
	 * \param soils soil class map
	 * \param disturbance map with the effect size of each disturbance (e.g. "mouse: 0.5, fire: 2")
	 * \param init is this function used during initialization?
	 */
	void createInputMaps(const Landscape<std::string>& soils, 
						const Landscape<std::map<std::string, double>>& disturbance, bool init);
	
	

	

	/**
	 * \brief get abundance of one FG from succession models.
	 * 
	 * \param PFG_id number of the PFG to fetch
	 * @return Landscape<double> abundance map of the PFG
	 */
	Landscape<double> getPFGabund(int PFG_id);

	/**
	 * \brief save abundance of one PFG. 
	 * \details checks whether it is the right time to save the data(i.e., the year is part of config::saveYears), 
	 * then saves the abundance map of the PFG to a json file.
	 * \param PFG name of the PFG to save
	 * \param year year to save
	 * \param fileName name of the file to save to. defaults to "<PFG>_<year>.json"
	*/
	void savePFG(const std::string& pfg, int year, std::string fileName = "");

	/**
	 * \brief save abundance of one PFG. 
	 * \details saves the abundance map of the PFG to a json file.
	 * does not make use of config::saveYears; use overload savePFG(pfg_name, year) instead if required.
	 * \note calls the private overload savePFG(int), which is awkward (see issue #28). 
	 * In the future we should do without the int function.
	 * \param pfg name of the PFG to save
	 * \param fileName name of the file to save to. defaults to "<pfg>.json"
	*/
	void savePFG(const std::string& pfg, std::string fileName = "");


	/**
	 * \brief save abundance of all PFGs. 
	 * \details saves the abundance of all PFGs in all cells to a json file. Output 
	 * format is: {cell1: {PFG_1: 4, PFG_2: 3}, cell2: {PFG_1: 4, PFG_2: 3}, ...}
	 * \param fileName name of the file to save to. defaults to "biomass.json"
	*/
	void saveAll(std::string fileName = "");

	/**
	 * \brief save abundance of all PFGs. 
	 * \details checks whether it is the right time to save the data(i.e., the year is part of config::saveYears), 
	 * then saves the abundance of all PFGs in all cells to a json file. Output 
	 * format is: {cell1: {PFG_1: 4, PFG_2: 3}, cell2: {PFG_1: 4, PFG_2: 3}, ...}
	 * \param year year to save
	 * \param fileName name of the file to save to. defaults to "biomass_<year>.json"
	*/
	void saveAll(int year, std::string fileName = "");

//==============================================================================
	private:

	const Inputs<GSP_PLANTS, Data_PLANTS>* m_plantInputs_ptr;
	std::map<std::string, Landscape<int>> m_SeedMap;
	Landscape <Cell*> m_cells;

	
/*----------------------------------------------------------------------------*/
// functions 

	/*!
	*  \brief Do succession (demographic) model within each pixel
	* \details passes current seeds to Cell to run the succession model; and updates the
	* seed map. Requires some conversion in hte process
	*/
	void DoSuccession(std::string cell, const std::vector<int>& neighbour);
	void Dosuccession(std::string cell);

	/*!
	*  \brief Do dispersal model within each pixel
	* \details passes current seeds to DoDispersalPacket to run the dispersal model.
	*/
	void disperse();


	
	/**
 	* \brief disperse seed randomly on the site
 	* \details The seeds produced in all cells of the site are summed up, and then placed 
 	* on the site randomly, following uniform distribution.
	disperse calls this function. In the future it may instead pass on seeds 
	to a separate dispersal model (like it was in RFAtE), but currently dispersal
	is only a single function. 
 	* 
 	* \param m_SeedMapIn current seed distribution (format {PFG_1: 4, PFG_2: 3})
 	* @return new seed distribution (same format as input)
 	*/
	std::map<std::string, Landscape<int>> DoDispersalPacket(const std::map<std::string, Landscape<int>>& m_SeedMapIn, int regionalSeeds);
	
	/**
	 * \brief save abundance of one PFG. 
	 * \details saves the abundance map of the PFG to a json file.
	 * \note calling a PFG by number is awkward and prone to errors (see issue #28).
	 * \param id number of the PFG to fetch
	 * \param fileName name of the file to save to. defaults to "<PFG>.json"; <PFG> is the name of the <id>th position in PFGDefs
	 * (which is supposed to be sorted lexicographically)
	*/
	void savePFG(int id, std::string fileName = "");

};

#endif