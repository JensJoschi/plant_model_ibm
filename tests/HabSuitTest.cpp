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
 // Jens Joschinski (EPM)
 // --------------------------------------------------------------------------

// This file includes unit tests for the class HabSuit. See tests/CMake file 
// and google test documentation.

#include "HabSuit.h"
#include "SoilRequirements.h"

/** @cond */
#include "gtest/gtest.h"
/** @endcond */

class HabSuitTest : public ::testing::Test {
  protected:
  HabSuit* h;
  static std::shared_ptr<Soil> soil;

  static void SetUpTestSuite() { //creates shared trait Definitions for all tests
    soil = std::make_shared<Soil>(3,10,"sand");
  }
};

std::shared_ptr<Soil> HabSuitTest::soil = nullptr;


TEST_F(HabSuitTest, allValid){
        nlohmann::json j = {
        {"minDepth", 10},
        {"acceptedSoils", {{"sand", true}, {"clay", false}}},
    };
    SoilRequirements soilReqs(j);
    h = new HabSuit(&soilReqs, soil);
    EXPECT_TRUE(h->isSuitable());
}

TEST_F(HabSuitTest, shallowSoil){
    nlohmann::json j = {
        {"minDepth", 50},
        {"acceptedSoils", {{"sand", true}, {"clay", false}}},
    };
    SoilRequirements soilReqs(j);
    h = new HabSuit(&soilReqs, soil);
    EXPECT_FALSE(h->isSuitable());
}

TEST_F(HabSuitTest, wrongSoil){
    nlohmann::json j = {
        {"minDepth", 10},
        {"acceptedSoils", {{"sand", false}, {"clay", false}}},
    };
    SoilRequirements soilReqs(j);
    h = new HabSuit(&soilReqs, soil);
    EXPECT_FALSE(h->isSuitable());
}

TEST_F(HabSuitTest, shallowAndWrongSoil){
    nlohmann::json j = {
        {"minDepth", 50},
        {"acceptedSoils", {{"sand", false}, {"clay", false}}},
    };
    SoilRequirements soilReqs(j);
    h = new HabSuit(&soilReqs, soil);
    EXPECT_FALSE(h->isSuitable());
}

TEST_F(HabSuitTest, soilChanges){
    nlohmann::json j = {
        {"minDepth", 10},
        {"acceptedSoils", {{"sand", true}, {"clay", false}}},
    };
    SoilRequirements soilReqs(j);
    h = new HabSuit(&soilReqs, soil);
    EXPECT_TRUE(h->isSuitable());
    soil->m_name = "clay";
    EXPECT_FALSE(h->isSuitable());
}

TEST_F(HabSuitTest, unknownSoil){
    nlohmann::json j = {
        {"minDepth", 10},
        {"acceptedSoils", {{"sand", true}, {"clay", false}}},
    };
    SoilRequirements soilReqs(j);
    soil->m_name = "loam";
    EXPECT_THROW(h = new HabSuit(&soilReqs, soil), std::runtime_error);
}