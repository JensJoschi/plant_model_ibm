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
// Jens Joschinski
// ----------------------------------------------------------------------------
#ifndef PLANTS_H
#define PLANTS_H

#include <memory>
#include "Landscape.h"

#include <string>
#include <map>

class PlantModel;

class Plants {
public:
	explicit Plants(const std::string& inputFile, const std::string& logConfig = "", bool FixRNG = false);
    ~Plants();

	/**
	 * \brief Create stable starting community
	 * \details
	 * Runs the pland model for "years" timesteps to initiate 
	 * a community, doing only succession and dispersal. 
     * Also loads disturbances and soil classes from file, if activated. 
	 * \param years number of time steps to run
	 */
    void initialize(int years);

    /**
	 * \brief Perform one plant time step
	 * \details one plant time step consists of plant succession in each cell, 
     * and of dispersal of seeds across the landscape. In detail, succession consists 
     * of the following steps:
	* 1. Disturbance
    * 2. Death if light is insufficient
    * 3. Aging
    * 4. Recalculation of light
    * 5. Germination and recruitment
    * 6. Dispersal
	 */
    void TPlusOne_JJ();

    /**
	 * \brief create inputs for plant model
	 * \details provide plant model with soil classes and disturbances. This function
     * is called once at initialization, and can be called again if disturbances/soils changed over time.
	 * \param soils soil class map
	 * \param disturbance map with the effect size of each disturbance (e.g. "mouse: 0.5, fire: 2")
	 * \param init is this function used during initialization?
	 */
    void createInputMaps(const Landscape<std::string>& soils, const Landscape<std::map<std::string, double>>& disturbance, bool init);
    
	/**
	 * \brief get abundance of one FG.
     * \note ID number is the nth element of PFGDefinitions. PFGDefinitions still uses a vector
     * to store the data (not a map). PFGs can usually be assumed to be sorted lexicographically, 
     * but this not guaranteed. see issue #28
	 * 
	 * \param PFG_id number of the PFG to fetch
	 * @return Landscape<double> abundance map of the PFG
	 */
    Landscape<double> getPFGabund(int PFG_id);

	/**
	 * \brief save abundance of one PFG. 
	 * \details checks whether it is the right time to save the data(i.e., the year is part of gsp::saveYears), 
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

private:
    std::unique_ptr<PlantModel> pImpl; // pointer to the implementation
};

#endif //PLANTS_H