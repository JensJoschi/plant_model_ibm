/*------------------------------------------------------------------------------
Copyright (C)  2022 - present  Studio Animal-Aided Design

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


/*!
 * \file p_PFGDefs.h
 * \brief Class for defining PFGs
 * \details This class contains the definitions of all potential plant functional groups.
 PFGDefinitions contains a std::map of PFG objects that store the information about 
 Plant Functional Groups, as well as the number of Functional Groups and their names. 
 A set of PFG definitions is automatically built by using the default constructors( `PFGDefs mydefinitions;` ), 
 or a json file can be provided.
 \note this class should probably be a map, not a vector.
 * \author Jens Joschinski
 * \version 1.0
 * \date 2022
*/

#ifndef PFGDEFS_H
#define PFGDEFS_H
/** @cond */
#include <map>
#include <string>
#include <vector>
/** @endcond */

#include "PFG.h"
#include "FGUtils.h"
#include "PFGdisturbance.h"
#include "FuncGroup.h"
 /*!
 * \class PFGDefs
 * \brief lists all Plant Functional Groups

 */
class PFGDefs{
	public:

//==============================================================================
//Constructors
//==============================================================================
	/*! 
	* \brief Default constructor
	* \details
	This constructor creates a list of plant functional groups that has been provided by the developers. 
	Currently this list contains only a few items for testing,later it may contain a whole set of standard PFGs.
	*/
	PFGDefs();

	/*! 
	* \brief json-file based constructor
	* \details this constructor opens the file (pathtodfes), and construct as many PFGs as there are objects contained in it.
	Matching disturbances are added from the file pathtodisturbances.
	 WARNING: there is no full error checking yet in PFG class. If attributes are wrong, the program may crash.
	* \param pathtodefs filename of json containing PFG definitions
	* \param pathtodisturbances filename of json containing PFG disturbance definitions
	*/
	explicit PFGDefs(const std::string& pathtodefs, const std::string& pathtodisturbances); 

	/*! 
	* \brief json-file based constructor without disturbance
	* \details this constructor opens the file (pathtodfes), and construct as many PFGs as there are objects contained in it
	* no disturbances are added.
	 WARNING: there is no full error checking yet in PFG class. If attributes are wrong, the program may crash.
	* \param pathtodefs filename of json containing PFG definitions
	*/
	explicit PFGDefs(const std::string& pathtodefs);


	/*! 
	* \brief semi-full constructor: direct passing of PFG objects. 
	* \details Essentially a copy constructor. Only used for development and debugging. 
	* \param pfgin map of PFGs
	* \param distin map of PFGDisturbances
	*/
	explicit PFGDefs(const std::map <std::string, PFG>& pfgin, const std::map <std::string, PFGDisturbances>& distin);

//============================================================================== 
//member functions
//==============================================================================

	int size() const;
	
	std::vector<std::string> getNames() const;

	/**
	 * \brief get all pfgs
	 * \details useful for iterating over all PFGs 
	 * for (auto& pfg : PFGdefs.getPFGs())
	 * @return const std::map<string, PFG>& 
	 */
	const std::map<std::string, PFG>& getPFGs() const;

	/**
	 * \brief get all disturbances
	 * \details useful for iterating 
	 */
	const std::map<std::string, PFGDisturbances>& getDisturbances() const;

	/**
	 * \brief get a pfg by ID
	 * \param name name of the PFG
	 * @return const PFG& 
	 */
	const PFG& getPFG(const std::string& name) const;

	const PFGDisturbances& getDisturbance(const std::string& name) const;

//==============================================================================
private:
	std::map<std::string, PFG> m_PFGs;
	std::map<std::string, PFGDisturbances> m_Disturbances;
	void createPFGs(const std::string& pathtodefs);
	void createDisturbances(const std::string& pathtodisturbances);
	void fillDisturbances();
	void check();

};
#endif