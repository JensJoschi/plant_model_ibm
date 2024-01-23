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
#include "jointModel/Landscape.h"

class PlantModel;

class Plants {
public:
	explicit Plants(const std::string& inputFile, const std::string& logConfig);
    ~Plants();

    void initialize(int years);
    void TPlusOne_JJ();
    void createInputMaps(const Landscape<std::string>& soils, const Landscape<std::map<std::string, double>>& disturbance, bool init);
    Landscape<double> getPFGabund(int PFG_id);

private:
    std::unique_ptr<PlantModel> pImpl; // pointer to the implementation
};

#endif //PLANTS_H