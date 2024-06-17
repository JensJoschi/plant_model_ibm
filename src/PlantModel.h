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
 // RFate team: provide original code, SimulMap (RFATE)
 // JJ: split SimulMap into a PlantModel class and a EcolopesLand class (ECOLOPES JOINT MODEL); 
 //     rewrote habitat suitability and disturbance functions (EPM)
 //     functions to exchange data with other models (ECOLOPES JOINT MODEL)
 // VMVC: functions to exchange data with other models (ECOLOPES JOINT MODEL)
 // JJ: rewrite (IBM)
 // ----------------------------------------------------------------------------

/*!
 * \file p_plantmodel.h
 * \brief main class of the plant model
 * \details this f
 */

#ifndef PLANTMODEL_H
#define PLANTMODEL_H

#include "Landscape.h"
#include "GSP_PLANTS.h"
#include "Data_PLANTS.h"
#include "Voxel.h"
#include "SeedDistribution.h"

/** @cond */
#include <vector>
#include <string>
#include <map>
#include "nlohmann/json.hpp"
/** @endcond */
#include "easylogging++.h"


/*!
 * \class PlantModel
 * 
  */
class PlantModel{

//==============================================================================
	public:
	/**
	 * \brief constructor

	 */
	explicit PlantModel(const nlohmann::json& globalParams, 
						const nlohmann::json& voxelData, 
						const nlohmann::json& TraitDefinitions, 
						const std::string& logConfig = "", bool FixRNG = false);

	/**
	 * \brief Perform one plant time step
	 */
	void TPlusOne();

//to do:
	/**
	 * \brief change current soil
	 * \details changes the soil in each cell
	 * \note currently only changes the name attribute of existing soils, could also replace the complete soil object
	 * this would allow changing e.g. depth or soil capacity.
	 * \param soils 
	 */
	// void changeSoil(const Landscape<std::string>& soils);
	// void changeDisturbances(const Landscape<std::map<std::string, double>>& disturbances);
	// void addDisturbance(const Landscape<double>& disturbance, const std::string& disturbanceName);

	void addSeedsFromRegion(int number);

	// Landscape<double> getBiomass(const std::string& type) const;
	Landscape<int> getNumber(const std::string& type) const;
	// Landscape<nlohmann::json> getInfo (const std::string& type) const;
	Landscape<float> getArea() const;
	Landscape<std::map<std::string, int>> getNumber() const;
	//this overload allows ignoring the layer of 0cm, i.e. non-germinated seeds
	Landscape<std::map<std::string, int>> getNumber(int from, int to) const;
	Landscape<std::map<std::string, float>> getBiomass() const;

	void save	 (int t, const std::string& fileName = "",bool sum = true, bool ordered = true) const;
	void saveArea(int t, const std::string& fileName = "") const;
//==============================================================================
	private:

	const GSP_PLANTS m_settings;
	const Data_PLANTS m_voxelData;
	const std::map<std::string, Traits> m_traits;
	SeedDistribution m_seeds;
	Landscape <Voxel*> m_cells;

	/*!
	*  \brief Do dispersal model within each voxel
	*/
	// void disperse();

	std::map<std::string, Traits> fillTraits (const nlohmann::json& TraitDefinitions);
};

#endif