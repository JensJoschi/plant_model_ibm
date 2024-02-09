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
#include "p_PlantModel.h" //implementation
#include "plants/plants.h" //public API
#include "easylogging++.h"
#include "config.h"

INITIALIZE_EASYLOGGINGPP

Plants::Plants(const std::string& inputFile, const std::string& logConfig, bool FixRNG){
    pImpl = std::make_unique<PlantModel>(inputFile, logConfig, FixRNG);
}

Plants::~Plants() = default;

void Plants::initialize(int years) {
    pImpl->initialize(years);
}

void Plants::TPlusOne_JJ() {
    pImpl->TPlusOne_JJ();
}

void Plants::createInputMaps(const Landscape<std::string>& soils, const Landscape<std::map<std::string, double>>& disturbance, bool init) {
    pImpl->createInputMaps(soils, disturbance, init);
}

Landscape<double> Plants::getPFGabund(int PFG_id) {
    return pImpl->getPFGabund(PFG_id);
}

void Plants::savePFG(const std::string& pfg, int year, std::string fileName) {
    pImpl->savePFG(pfg, year, fileName);
}

void Plants::savePFG(const std::string& pfg, std::string fileName) {
    pImpl->savePFG(pfg, fileName);
}

void Plants::saveAll(std::string fileName) {
    pImpl->saveAll(fileName);
}

void Plants::saveAll(int year, std::string fileName) {
    pImpl->saveAll(year, fileName);
}