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
 // Jens Joschinski (IBM)
 // --------------------------------------------------------------------------

// This file includes unit tests for the class Individual. See tests/CMake file 
// and google test documentation.

#include "Individual.h"
#include "Traits.h"

/** @cond */
#include "gtest/gtest.h"
/** @endcond */

class IndividualTest : public ::testing::Test {
  protected:
  Individual* i;
  static const Traits* traits;
  static std::shared_ptr<Soil> soil;

  static void SetUpTestSuite() { //creates shared trait Definitions for all tests
    nlohmann::json j = {
        {"minDepth", 10},
        {"acceptedSoils", {{"sand", true}, {"clay", false}}},

        {"conversionEfficiency", 1.0},
        {"maintenanceCosts", 0.1},
        {"seedAllocation", 0.01},
        {"biomassAllocation", 0.5},
        {"maxInvestment", 0.05},

        {"Dormancy", false},
        {"MortalityActive", 0.1},
        {"MortalityDormant", 0.05},
        {"ActivationRate", 0.8},

        {"MaturationTime", 10},
        {"LifeSpan", 20},
        {"MaxHeight", 8},
        {"ShadeFactor", 0.5}
    };
    traits = new Traits(j);
    soil = std::make_shared<Soil>(3,10,"sand");
    }

  static void TearDownTestSuite() {
    delete traits;
    traits = nullptr;
  }

  void SetUp() override { 
    std::shared_ptr<Soil> soil_ptr(soil);
    i = new Individual(traits, soil_ptr);
  }
  void TearDown() override {
    delete i;
    i = nullptr;
  }
};

const Traits* IndividualTest::traits = nullptr;
std::shared_ptr<Soil> IndividualTest::soil = nullptr;

TEST_F(IndividualTest, builds){
    EXPECT_TRUE(i->m_LifeHist_ptr);
    EXPECT_TRUE(i->m_SoilReq_ptr);
    EXPECT_EQ(i->m_age, 0);
    EXPECT_EQ(i->m_biomass, 0);
    EXPECT_TRUE(i->m_resPool_ptr);
}

TEST_F(IndividualTest, getBiomass){
  EXPECT_EQ(i->getBiomass(), 0);
  i->m_height = 10;
  i->m_biomass = 60;
  EXPECT_EQ(i->getBiomass(), 60);
  EXPECT_EQ(i->getBiomass(0,10), 60);
  EXPECT_EQ(i->getBiomass(0,5), 30);
  EXPECT_EQ(i->getBiomass(5,10), 30);
  EXPECT_EQ(i->getBiomass(3,7), 24);
  EXPECT_EQ(i->getBiomass(3,3), 0);
  EXPECT_EQ(i->getBiomass(0,0), 0);
}

TEST_F(IndividualTest, doesItDie){
  EXPECT_TRUE(i->doesItDie());
  i->m_resPool_ptr->updateResource(400);
  EXPECT_FALSE(i->doesItDie());
}

TEST_F(IndividualTest, feed){
    EXPECT_TRUE(i->doesItDie());
    i->feed(0);
    EXPECT_TRUE(i->doesItDie());
    i->feed(10);
    EXPECT_EQ(i->getBiomass(), 0); //resources not converted to biomass yet
    EXPECT_FALSE(i->doesItDie());
}

TEST_F(IndividualTest, disturbWithoutRange){
    i->m_biomass = 100;
    int deficit = i->disturb(50);
    EXPECT_EQ(deficit, 0);
    EXPECT_EQ(i->m_biomass, 50);

    deficit = i->disturb(60);
    EXPECT_EQ(deficit, 10);
    EXPECT_EQ(i->m_biomass, 0);

    deficit = i->disturb(0);
    EXPECT_EQ(deficit, 0);
    EXPECT_EQ(i->m_biomass, 0);
}

TEST_F(IndividualTest, disturbWithRange){
    i->m_biomass = 100;
    i->m_height = 10;
    int deficit = i->disturb(30, 3, 7); // can access 40% of the biomass
    EXPECT_EQ(deficit, 0);
    EXPECT_EQ(i->m_biomass, 70);

    deficit = i->disturb(70, 0, 5); //can access half of the biomass
    EXPECT_EQ(deficit, 35);
    EXPECT_EQ(i->m_biomass, 35);
}

// TEST_F(IndividualTest, deplete){
//   EXPECT_TRUE(i->doesItDie());
//   i->feed(60);
//   int result = i->depleteResources(0);
//   EXPECT_FALSE(i->doesItDie());
//   result = i->depleteResources(61);
//   EXPECT_EQ(result,1);
// }

TEST_F(IndividualTest, age){
  EXPECT_EQ(i->m_age, 0);
  int newSeeds = i->age(); //no resources available
  EXPECT_EQ(i->m_age, 1);
  EXPECT_EQ(newSeeds, 0);
  EXPECT_EQ(i->m_biomass, 0);
  i->m_biomass = 100;
  i->feed(120);
  newSeeds = i->age();
  EXPECT_EQ(i->m_age, 2);
  EXPECT_EQ(newSeeds, 1);
  EXPECT_EQ(i->m_biomass, 105); //seee ResourceTest

  i->feed(10000000);
  i->m_biomass = 1000;
  newSeeds = i->age();
  EXPECT_EQ(i->m_age, 3);
  EXPECT_EQ(newSeeds, 100000);
  EXPECT_EQ(i->m_biomass, 1050);//max investment
}     