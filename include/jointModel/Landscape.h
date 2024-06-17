/*Copyright (C)  2022 - present  Studio Animal-Aided Design

This file is part of the INDIVIDUAL-BASED PLANT MODEL.

INDIVIDUAL-BASED PLANT MODEL is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your option) any later version.

INDIVIDUAL-BASED PLANT MODEL is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with INDIVIDUAL-BASED PLANT MODEL.
If not, see <https://www.gnu.org/licenses/>. */

/*This file is based on a file from the ECOLOPES JOINT MODEL, Copyright (C) 2022 - present Studio Animal-Aided Design and TU Munich

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
#include <set>
/** @endcond */

/**
 * \brief Coordinate class
 * \details Stores x, y, z coordinates of a cell, used by the Landscape class.
 */
class Coordinates {
	template <typename T> friend class Landscape;
	int x; 
	int y; 
	int z;

	public:
	Coordinates() = default;
	Coordinates(int x, int y, int z): x(x), y(y), z(z) {}

	explicit Coordinates(const std::string& key){
		checkKey(key);
		std::string_view key_view(key);
		size_t pos = key_view.find(",");
		x = std::stoi(std::string(key_view.substr(1, pos-1)));
		key_view.remove_prefix(pos+2);
		pos = key_view.find(",");
		y = std::stoi(std::string(key_view.substr(0, pos)));
		key_view.remove_prefix(pos+2);
		z = std::stoi(std::string(key_view.substr(0, key_view.size()-1)));
	}

	std::string to_string() const {return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";}

	friend std::ostream& operator<<(std::ostream& os, const Coordinates& coords) {
	return os << coords.to_string();
	}

	bool operator<(const Coordinates& other) const { //for std::less operator, needed for std::map
		if (x < other.x) return true;
		if (x > other.x) return false;
		if (y < other.y) return true;
		if (y > other.y) return false;
		return z < other.z;
	}

	private:
	void checkKey(const std::string& key){
		std::string_view key_view(key);
		if (key_view.find_first_not_of("0123456789-() ,") != std::string::npos){
			LOG(FATAL) << "Error: key " << key << " is not allowed. wrong characters. ";}
		size_t pos = key_view.find(",");
		if (pos == std::string::npos) LOG(FATAL) << "Error: key " << key << " is not allowed. Missing comma. ";
		if (key_view[0] != '(' || key_view[key_view.size()-1] != ')') LOG(FATAL) << "Error: key " << key << " is not allowed. Missing brackets. ";
		if (key_view[pos+1] != ' ') LOG(FATAL) << "Error: key " << key << " is not allowed. Missing space after x coord. ";
	}
};

 /*!
 * \class Landscape
 * \brief Stores data(e.g. shading) of the site. 
 * \details 
 * The data is stored as std::maps, where keys are coordinates. 
 * Values could be int or double, string, or they could themselves be maps or vectors.
 * The map can have "holes", i.e. not all possible x-y-z combinations need to be present.
 * However, once constructed, the keys can no longer change, thus it is guaranteed that space
 * does not get corrupted.
 */
template <class val_T>
class Landscape{
	std::map <Coordinates, val_T> m_values;
	
	public:
	/*-------------------------------------------*/
	/* -------- Constructors  ------*/
	/*-------------------------------------------*/
	Landscape() = default;

	explicit Landscape (std::vector<Coordinates> keys){
		assert(keys.size()!= 0);
		for (const auto& key : keys) {m_values[key] = val_T();} //0 for int, "" for string etc.
	}

	explicit Landscape(const std::map<Coordinates, val_T>& map): m_values(map){
		assert(m_values.size()!= 0);
	}

	/**
	 * \brief json-based constructor
	 * \details Json file example: {"(0, 4, 7)": 7, "(0, 5, 7)": 6}
	 * Instead of values, one may also provide an array of size 1: {"(0, 4, 7)": [7], "(0, 5, 7)": [6]}
	 */
	explicit Landscape(const nlohmann::json& j){
		assert (j.size() != 0);
		for (const auto& [key, value] : j.items()) {
			Coordinates c(key);
			if (value.is_array() && value.size() == 1){
					m_values[c] = value[0];
			} else if (value.is_array() && value.size() > 1) {
				LOG(FATAL) << "Error: key " << key << " contains more than one value. ";
			} else {
			   m_values[c] = value;
			}
		}
	}

	/**
	 * \brief json-based constructor
	 * \details Json file example: {"(0, 4, 7)": {"a": 4, "b":3.2}, "(0, 5, 7)": {"a": 3, "b":6.2}}. 
	 *  With info = "b" the values will be 3.2 and 6.2
	 *  Ctor also supports alternative notation with array of size 1: 
	 *  {"(0, 4, 7)": [{"a": 4, "b":3.2}], "(0, 5, 7)": [{"a": 3, "b":6.2}]}
	 */
	explicit Landscape(const nlohmann::json& j, std::string_view info){
		assert (j.size() != 0);
		for (const auto& [key, value] : j.items()) {
			Coordinates c(key);
			if (value.is_array() && value.size() == 1) { //[{"a": 4, "b":3.2}]
				auto val = value[0].find(info);
				m_values.insert(std::pair<Coordinates, val_T>(c, val->template get<val_T>()));
			}
			else if (value.is_object()){	//{"a": 4, "b":3.2}
				auto val = value.find(info);   
				m_values.insert(std::pair<Coordinates, val_T>(c, val->template get<val_T>()));
			}
			else { LOG(FATAL) << "Error: json does not have the expected format. Please check!";}
		}
	}

	/*-------------------------------------------*/
	/* Iterators etc       ----------------------*/
	/*-------------------------------------------*/

	auto begin(){return m_values.begin();}
	auto end(){return m_values.end();}
	auto cbegin() const {return m_values.begin();}
	auto cend() const {return m_values.end();}
	auto begin() const {return m_values.begin();}
	auto end() const {return m_values.end();}
	size_t count(const Coordinates& key) const {return m_values.count(key);}
	bool contains(const std::string& key) const {return m_values.contains(Coordinates{key});}
	const size_t size() const {return m_values.size();}

	/*-------------------------------------------*/
	/* functions          s----------------------*/
	/*-------------------------------------------*/

	const nlohmann::json get_json() const{
		assert(m_values.size()!= 0);
		nlohmann::json j;
		for (const auto& x : m_values) { j[x.first.to_string()] = x.second;}
		assert(j.size() != 0);
		return j;
	}

	const val_T& at(const Coordinates& x) const{
		assert(m_values.size()!= 0);
		return (m_values.at(x));
	}

	void setValue(const Coordinates& c, const val_T& value){
		assert(m_values.size()!= 0);
		if (!m_values.contains(c)) LOG(FATAL) << "coordinate "<< c<< " does not exist";
		m_values.at(c) = value;
	}

	void fillWith(const val_T& value){
		assert(m_values.size()!= 0);
		for (auto& x : m_values) { x.second = value;}
	}

	std::vector<Coordinates> getNeighbours2D(const Coordinates& c) const {
		assert(m_values.size()!= 0);
		std::vector<Coordinates> neighbours;
		for (int i = -1; i <= 1; i++) {
			for (int j = -1; j <= 1; j++) {
				if (i == 0 && j == 0) continue;
				Coordinates n {c.x + i, c.y + j, c.z};
				if (m_values.contains(n)) neighbours.push_back(n);
			}
		}
		return neighbours;
	}

	/**
	 * \brief Get the 2D neighbours of a cell
	 * \details Calculates the immediate neighbours (level 1), then the neighbours' 
	 * neighbours (level 2) etc. Neighbours that are already in the list are not added again.
	 * Output is sorted by levels (std::vector<Neighbours>[0] = level 1...).
	 * \param c 
	 * \param levels 
	 * @return std::vector<std::vector<Coordinates>> 
	 */
	std::vector<std::vector<Coordinates>> getNeighbours2D(const Coordinates& c, int levels) const{
		assert(m_values.size()!= 0);
		assert(levels>0);
		std::vector<std::vector<Coordinates>> all_neighbours;
		std::set<Coordinates> unique_neighbours;
		unique_neighbours.insert(c);
		auto first_level_neighbours = getNeighbours2D(c);
		all_neighbours.push_back(first_level_neighbours);
		unique_neighbours.insert(first_level_neighbours.begin(), first_level_neighbours.end());
		for (int i = 1; i < levels; i++) {
			std::vector<Coordinates> new_neighbours;
			for (const auto& neighbour : all_neighbours[i-1]) {
				auto n = getNeighbours2D(neighbour);
				for (const auto& new_neighbour : n) {
					if (unique_neighbours.find(new_neighbour) == unique_neighbours.end()) {
						new_neighbours.push_back(new_neighbour);
						unique_neighbours.insert(new_neighbour);
					}
				}
			}
			all_neighbours.push_back(new_neighbours);
		}
		return all_neighbours;
	}

	std::vector<val_T> getNeighbourValues2D(const Coordinates& c) const {
		assert(m_values.size()!= 0);
		std::vector<val_T> neighbour_values;
		for (const auto& neighbour : getNeighbours2D(c)) {
			neighbour_values.push_back(m_values.at(neighbour));
		}
		return neighbour_values;
	}

	/*-------------------------------------------*/
	/*  Template specializations ----------------*/
	/*-------------------------------------------*/
	const unsigned getNoVals() const { 
		assert (false && "tried calling Landscape::getNoVals function, but T is not a map");
	}
	void insertValue(const Coordinates& c, std::pair<const std::string&, double> element){
		assert(false &&  "tried calling Landscape::insertValue function, but T is not a map");
	}
	void replaceValue(const Coordinates& c, std::pair<const std::string&, double> element){
		assert(false &&  "tried calling Landscape::replaceValue function, but T is not a map");
	}
};

template<>
inline void Landscape<std::map<std::string, double>>::insertValue(const Coordinates& c, std::pair<const std::string&, double> element){
	assert(m_values.size()!= 0);
	if (!m_values.contains(c)) LOG(FATAL) << "coordinate " << c << " does not exist";
	if (m_values[c].contains(element.first)) {
		LOG(FATAL) << "error when inserting inner map element to a landscape<map>:  " << element.first << " already exists";
	}
	m_values[c].insert(element);
}

template<>
inline void Landscape<std::map<std::string, double>>::replaceValue(const Coordinates& c, std::pair<const std::string&, double> element){
	assert(m_values.size()!= 0);
	if (!m_values.contains(c)) LOG(FATAL) << "coordinate " << c << " does not exist";
	if (!m_values[c].contains(element.first)) {
		LOG(FATAL) << "error when replacing inner map element of a landscape<map>:  " << element.first << " does not exist";
	}
	m_values[c][element.first] = element.second;
}

template<>
inline const unsigned Landscape<std::map<std::string, double>>::getNoVals() const {
	assert(m_values.size()!= 0);
	return(m_values.begin()->second.size()); //return number of values in first cell only (should be same for all cells)
};

#endif