/* Copyright (C) 2022 - present Studio Animal-Aided Design and TU Munich
 This file is part of the ECOLOPES JOINT MODEL.

 ECOLOPES JOINT MODEL is free software: you can redistribute it and/or modify 
 it under the terms of the GNU General Public License as published by the 
 Free Software Foundation, either version 3 of the License, or (at your option) any later version.

 ECOLOPES JOINT MODEL is distributed in the hope that it will be useful, 
 but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with ECOLOPES PLANT MODEL. 
 If not, see <https://www.gnu.org/licenses/>.

 ECOLOPES JOINT MODEL is based on:
 - the ECOLOPES PLANT MODEL, 2022 - present  Studio Animal-Aided Design
 - the ECOLOPES ANIMAL MODEL, Copyright (C) 2022 - present TU Munich
 - and the ECOLOPES SOIL MODEL,2022 - present  Studio Animal-Aided Design

*/

// --------------------------------------------------------------------------
// Authors and contributors to this file:
// Jens Joschinski
// --------------------------------------------------------------------------

#ifndef LANDSCAPE_H
#define LANDSCAPE_H

/*!
 * \file Landscape.h
 * \brief Stores data(e.g. shading) of the site.
 * \author Jens Joschinski
 * \version 1.0
 */
/** @cond */
#include <iostream>
#include <fstream>
#include <vector>
#include <string_view>
#include <map>
#include <string>
#include <nlohmann/json.hpp>
#include "easylogging++.h"
/** @endcond */



 /*!
 * \class Landscape
 * \brief Stores data(e.g. shading) of the ecolope site. 
 * \details 
 * The data is stored as std::maps, usually with keys of the form "(x,y,z)", where x,y,z are 
 * coordinates. Values could be int or double, string, or they could themselves be maps or vectors.
  * The map can have "holes", i.e. not all possible x-y-z combinations need to be present.
 * However, once constructed, the keys can no longer change, thus it is guaranteed that space
 * does not get corrupted.
 * \note in the future we may relax the requirement that keys need to have a specific format.
 */
template <class val_T>
class Landscape{
	public:
	/*-------------------------------------------*/
	/* Constructors -----------------------------*/
	/*-------------------------------------------*/
	//the following constructors exist
	//provide nothing			-> default constructor, contains no keys. Accessing this Landscape's values will result in an error.
	//provide a vector of keys	-> create landscape with these keys and fill with null values (e.g. empty or 0)
	//provide map&				-> create landscape
	//provide filename			-> read json file and create landscape
	//provide filename, variable-> read json file and extract one variable (double or int); create landscape
	// all ctors except the default will also check that the keys are in the right range

	/*!
	 *	\brief Default constructor
	 \details this constructor creates a landscape with spatial extent of 0 (const), so any use of this map will throw an error. 
	 The default constructor is used when some inputs cannot be read or are not provided.
	 */
	Landscape() = default;

	/**
	 * \brief Constructor with set of keys
	 * \details creates a Landscape object with a set of keys. The keys are checked for correct range, and values are set to 
	 * default values (e.g. 0 for int, "" for string) 
	 * \param keys the coordinates, usually formatted as "(x,y,z)"
	 */
	Landscape (std::vector<std::string> keys){
  		for (const auto& key : keys) {m_values[key] = val_T();}
		checkSize();
		checkKeys();
	}

	/*!
	 *	\brief std::map-based constructor
	 *  \details copies a std::map into the Landscape object. Essentially a copy constructor.
	 * 	\param map : std::map containing all values to store
	 */
	Landscape(const std::map<std::string, val_T>& map): m_values(map){
		checkSize();
		checkKeys();
	}


	/**
	 * \brief Construct a new Landscape from file
	 * \details the Json file contains keys in the form "(x,y,z)" and values of type val_T: "(x,y,z)": 4
	 * Instead of values, one may also provide an array of size 1: "(x,y,z)": [4]
	 * \param file a file name
	 * \note do not use this funtion with complex container types (vectors, maps, etc.)
	 */
	Landscape(const std::string& file){
		LOG(DEBUG) << "File name: " <<  file;
		std::ifstream f(file.c_str());
		if (!f.good()) LOG(FATAL) << "the file " << file << " cannot be opened. Please check!";
		nlohmann::json j = nlohmann::json::parse(f);

  		for (const auto& [key, value] : j.items()) {
    		if (value.is_array() && value.size() == 1) {
				m_values[key] = value[0];
			} else {
			   m_values[key] = value;
			}
		}
		checkSize();
		checkKeys();
	}

	/**
	 * \brief Construct a new Landscape from json file, choosing a variable among the keys
	 * \details the Json file contains keys in the form "(x,y,z)" and values are json objects: "(x,y,z)": {"var1": 4, "var2": 5}
	 * Instead of json objects, one may also provide an array of size 1: "(x,y,z)": [{"var1": 4, "var2": 5}]. The function 
	 * looks for the variable "info" in the json object (e.g. "var2") and extracts its value.
	 * \param file  filename
	 * \param info the name of the variable to be extracted from the json object
	 */
	Landscape(const std::string& file, std::string_view info){
		LOG(DEBUG) << "File name: " << file;
		std::ifstream f(file.c_str());
		if (!f.good()) LOG(FATAL) << "the file " << file << " can not be opened. Please check!";
		nlohmann::json j = nlohmann::json::parse(f);

 	for (auto& x : j.items()){
			if (x.value().is_array() && x.value().size() == 1) { //x.value() has form [{"a": 4, "b":3.2}]
				auto value = x.value()[0].find(info);   
				m_values.insert(std::pair<std::string, val_T>(x.key(), value->get<val_T>()));
			}
			else if (x.value().is_object()){		//x.value() has form  {"a": 4, "b":3.2}
				auto value = x.value().find(info);   
				m_values.insert(std::pair<std::string, val_T>(x.key(), value->get<val_T>()));
			}
			else {
				LOG(FATAL) << "the json file " << file << " does not have the expected format. Please check!";
			}
	}
		checkSize();
		checkKeys();
	}


	/*-------------------------------------------*/
	/* Get Information about the landscape ------*/
	/*-------------------------------------------*/

	/**
	 * \brief Get the total extent of a landscape
	 */
	const unsigned getTotncell() const {
		return m_values.size();
	}

	std::vector<std::string> getKeys() const {
		std::vector<std::string> keys;
		for (const auto& [key, value] : m_values) {
			keys.push_back(key);
		}
	return keys;
	}



	/*-------------------------------------------*/
	/* std::map functions  ----------------------*/
	/*-------------------------------------------*/

	auto begin(){return m_values.begin();}
	auto end(){return m_values.end();}
	auto cbegin() const {return m_values.begin();}
	auto cend() const {return m_values.end();}
	auto begin() const {return m_values.begin();}
	auto end() const {return m_values.end();}

	size_t count(const std::string& key) const {return m_values.count(key);}
	bool contains(const std::string& key) const {return m_values.contains(key);}
	

	/*-------------------------------------------*/
	/* Get or change values----------------------*/
	/*-------------------------------------------*/

	const nlohmann::json get_json() const{
		assert(m_values.size()!= 0);
		nlohmann::json j;
		for (const auto& x : m_values) {
			j[x.first] = x.second;
		}
		assert(j.size() != 0);
		return j;
	}

	// val_T at(unsigned x, unsigned y, unsigned z = 0) const {
	// 	assert(m_values.size()!= 0);
	// 	return (m_values.at(tuple2str_vmvc(x, y, z)));
	// }

	val_T at(const std::string x) const {
		assert(m_values.size()!= 0);
		return (m_values.at(x));
	}

	// void setValue(unsigned x, unsigned y ,  const val_T& value, unsigned z = 0){
	// 	assert(m_values.size()!= 0);
	// 	m_values.at(tuple2str_vmvc(x,y,z)) = value;
	// }

	void setValue(const std::string& cell, val_T value){
		assert(m_values.size()!= 0);
		if (!m_values.contains(cell)) LOG(FATAL) << "cell "<< cell<< " does not exist";
		m_values.at(cell) = value;
	}

	void fillWith(val_T value){
		assert(m_values.size()!= 0);
		for (auto& x : m_values) {
			x.second = value;
		}
	}

	/*-------------------------------------------*/
	/* Other functions --------------------------*/
	/*-------------------------------------------*/

	/**
	 * \brief Write the landscape to a json file
	 * \param filename output filename
	 */
	const void write_json (const std::string& filename) const {
		assert(m_values.size()!= 0);
		nlohmann::json j = get_json();
		std::ofstream o(filename);
		if (!o.good()) LOG(FATAL) << "could not open file " << filename;
		o << std::setw(4) << j << std::endl;
	}
	
	//ignore these functions. used for template specialization.
	const unsigned getNoVals() const { 
		assert(m_values.size()!= 0);
		return (getTotncell());
	}
	void insertValue(const std::string& cell, const std::string& inner, double value){
		LOG(FATAL) << "tried calling insertValue function, but this function is only for std::maps";
	}
	void replaceValue(const std::string& cell,const std::string& inner, double value){
		LOG(FATAL) << "tried calling replaceValue function, but this function is only for std::maps";
	}
	void addElement(Landscape<std::string> &other, const std::string& name){
		LOG(FATAL) << "tried calling addElement function, but this function is only for std::maps";
	}


	private:
	std::map <std::string, val_T> m_values;
	/**
	 * \brief making sure the map fits expectations
	 * \details one can add assertions in this function, making e.g. sure that the map contains no holes 
	 * or has a certain specified size (e.g. 50*50*1)
	 */
	void checkSize() const {
		assert(m_values.size()!= 0);
	}

	void checkKeys(){
		for (const auto& key : getKeys()) {
			if (key.find_first_not_of("0123456789(), ") != std::string::npos) LOG(FATAL) << "Error: key " << key << " is not allowed ";
    		if (key.find("(") == std::string::npos || key.find(")") == std::string::npos || key.find(",") == std::string::npos) LOG(FATAL) << "Error: key " << key << " is not allowed ";
  		}
	}
	    /**
     * \brief create a key from x,y,z coorinates
     * \details Input data is of form "x, y, z": 0.4, [...]. The x,y,z are coordinates
     * This function creates a key from the coordinates, "(x, y, z)". If no z is provided,
     * the key is "(x, y, 0)".
     * \param x x-coordinate
     * \param y y-coordinate
     * \param z z-coordinate. defaults to 0
     * @return std::string the key
     */
	// std::string tuple2str_vmvc(int x, int y, int z = 0) const { return "(" + std::to_string(x)+", " + std::to_string(y) + ", " + std::to_string(z) + ")";}
};

//------------------------------------------------------------------------------
//template specializations for std::map

//currently not used, the soilmodel function to concatenate information from multiple strings is commented out
//requires a bit of work to remove the nox,noy stuff
/**
 * \brief construct landscape<map<string,string>> from a file
 * \details The file is expected to contain a nested json, where the first key 
 * is the cell name, e.g., "(0,0,0)", and the second key is the name of each parameter. 
 * The values are the values of the parameter.
 * The function also accepts an unnested json, in this case the values will be {"soilclass": value}
 * for each cell.
 * \note this function is currently only used for reading the soil class. requires generalization
 * (change of "soilclass")
 * 
 * \param nox 
 * \param noy 
 * \param dir 
 * \param file 
 * \param noz 
 */
// template<>
// inline Landscape<std::map<std::string, std::string>>::Landscape(int nox, int noy, const std::string& dir, const std::string& file, int noz):
// 	m_no_x(nox), m_no_y(noy), m_no_z(noz){
// 		checkSize();
// 		LOG(DEBUG) << "read maps from file";
// 		std::string filename = dir + file;
// 		LOG(DEBUG) << "File name: " <<  filename;
// 		std::ifstream f(filename.c_str());
// 		if (!f.good()) LOG(FATAL) << "the file "<< filename << " can not be opened. Please check!";
// 		try{
// 			nlohmann::json j = nlohmann::json::parse(f);

// 			for (auto& x : j.items()){
// 				std::string key = x.key();
// 				//check if value is a map <string, string> or only a string
// 				if (x.value().is_string()) {
// 					std::map<std::string, std::string> temp = {{"soilclass", x.value()}};
// 					m_values.insert(std::pair<std::string, std::map<std::string, std::string>>(key,temp));
// 				}
// 				else if (x.value().is_object()) {
// 					std::map<std::string, std::string> value;
// 					for (auto& y : x.value().items()){
// 						value.insert(std::pair<std::string, std::string>(y.key(), y.value()));
// 					}
// 					m_values.insert(std::pair<std::string, std::map<std::string, std::string>>(key,value));
// 				}
// 				else LOG(FATAL) << "error when parsing json file: " << x.value();
// 			}
// 		}
// 		catch (nlohmann::json::parse_error& e){
// 			LOG(FATAL)<< "error when parsing json file: ", e.what();
// 		}

// 		//check if all cells are present
// 		std::vector<std::string> keys;
// 		for (auto& x : m_values) {
// 			keys.push_back(x.first);
// 		}
// 		generalFunctions::checkKeys(keys, m_no_x, m_no_y, m_no_z);
// 	}


/**
 * \brief insert value to inner map element
 * \details access and modify the value of one of the std::map elements in a cell. 
 * \param cell which grid cell to acesss
 * \param inner which inner element to access
 * \param inner which inner element to access
 * \param value element that shuold be added to inner map
 */
template<>
inline void Landscape<std::map<std::string, double>>::insertValue(const std::string& cell, const std::string& inner, double value){
	assert(m_values.size()!= 0);
	if (!m_values.contains(cell)) {
		LOG(FATAL) << "cell " << cell << " does not exist";
	}
	if (m_values[cell].contains(inner)) {
		LOG(FATAL) << "error when inserting inner map element to a landscape<map>: key " << inner << " already exists";
	}
	m_values[cell][inner] = value;
}

/**
 * \brief replace inner map element
 * \details access and modify the value of one of the std::map elements in a cell.
 * \param cell which grid cell to acesss
 * \param inner which inner element to access
 * \param value replace value with this
 */
template<>
inline void Landscape<std::map<std::string, double>>::replaceValue(const std::string& cell, const std::string& inner, double value){
	assert(m_values.size()!= 0);
	if (!m_values.contains(cell)) {
		LOG(FATAL) << "cell " << cell << " does not exist";
	}
	if (!m_values[cell].contains(inner)) {
		LOG(FATAL) << "error when inserting inner map element to a landscape<map>: key " << inner << " does not exist";
	}
	m_values[cell][inner] = value;
}

/**
 * \brief add one element to a Landscape<map>
 * \details copies the values of "other" Landscape into this Landscape. Example: this Landscape may contain
 * (0,0,0): {"soilclass": "sand"}. other may have (0,0,0): "low", and "name" may be "structure". Then the result
 * of this landscape will be (0,0,0): {"soilclass": "sand", "structure": "low"}
 * \param other Landscape to add
 * \param name  name of the new element
 */
template<>
inline void Landscape<std::map<std::string, std::string>>::addElement(Landscape<std::string> &other, const std::string& name){
	assert(m_values.size()!= 0);
	for (auto it = m_values.begin(); it != m_values.end(); ++it){
		std::string cell = it->first;
		m_values[cell][name] = other.at(cell);
	}
}

/**
 * \brief get number of inner map elements
 * @return const unsigned number of inner map elements
 */
template<>
inline const unsigned Landscape<std::map<std::string, double>>::getNoVals() const {
	assert(m_values.size()!= 0);
	return(m_values.begin()->second.size()); //return number of values in first cell only (should be same for all cells)
};

#endif