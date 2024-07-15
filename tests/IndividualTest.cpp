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

// This file includes unit tests for the class Individual. See tests/CMake file 
// and google test documentation.

#include "Individual.h"
#include "Traits.h"

/** @cond */
#include "gtest/gtest.h"
#include <memory> 
#include <utility>
/** @endcond */

class IndividualTest : public ::testing::Test {
  protected:
  std::unique_ptr<Individual> i;
  static const Traits* traits;
  static std::shared_ptr<Soil> soil;

  static void SetUpTestSuite() { //creates shared trait Definitions for all tests
    nlohmann::json j = {
        {"minDepth", 10},
        {"acceptedSoils", {{"sand", true}, {"clay", false}}},
        {"size", 1},

        {"conversionEfficiency", 1.0},
        {"maintenanceCosts", 0.1},
        {"seedAllocation", 0.01},
        {"biomassAllocation", 0.5},
        {"maxBiomass", 1000},

        {"Dormancy", false},
        {"GerminationSuccess", 0.1},
        {"MortalityDormant", 0.05},
        {"DormancyBreakRate", 0.8},

        {"MaturationTime", 10},
        {"LifeSpan", 20},
        {"MaxHeight", 8},

        {"density", 2.0},

    };
    traits = new Traits(j);
    soil = std::make_shared<Soil>(4,10,"sand");
    }

  static void TearDownTestSuite() {
    delete traits;
    traits = nullptr;
  }

  void SetUp() override { 
    i = Individual::create(traits, soil, std::string{"testSpecies"});
  }
};

const Traits* IndividualTest::traits = nullptr;
std::shared_ptr<Soil> IndividualTest::soil = nullptr;

TEST_F(IndividualTest, builds){
    EXPECT_FALSE (i == nullptr);
    EXPECT_TRUE(i->m_growth_ptr);
    EXPECT_TRUE(i->m_habSuit_ptr);
    EXPECT_TRUE(i->m_resource_ptr);
    EXPECT_TRUE(i->m_shape_ptr);
    EXPECT_FALSE(i == nullptr);
    soil->m_name = "clay";
    std::unique_ptr<Individual> i2 = Individual::create(traits,soil,std::string{"testSpecies"});
    EXPECT_TRUE(i2 == nullptr);
    soil->m_name = "sand";
    EXPECT_EQ(i->m_species, "testSpecies");
}

TEST_F(IndividualTest, buildsWithJson){
  nlohmann::json correctVariables = {
    {"currGrowth",{
      {"height", 3.2},
      {"age", 4}
      }
    },
    {"currRes",{
      {"resources",  9999.0},
      {"biomass", 200.0}
      }
    },
    {"species", "testSpecies"}
    };
  nlohmann::json heightMissing = {
    {"currGrowth",{
      {"age", 4}
      }},
    {"currRes",{
      {"resources",  9999.0},
      {"biomass", 200.0}
      }
    },
    {"species", "testSpecies"}
    };
  nlohmann::json ageMissing = {
    {"currGrowth",{
      {"height", 3.2},
      }},
    {"currRes",{
      {"resources",  9999.0},
      {"biomass", 200.0}
      }
    },
    {"species", "testSpecies"}
    };
  nlohmann::json resourcesMissing = {
    {"currGrowth",{
      {"height", 3.2},
      {"age", 4}
      }},
    {"currRes",{
      {"biomass", 200.0}
      }
    },
    {"species", "testSpecies"}
    };
  nlohmann::json biomassMissing = {
    {"currGrowth",{
      {"height", 3.2},
      {"age", 4}
      }},
    {"currRes",{
      {"resources",  9999.0},
      }
    },
    {"species", "testSpecies"}
    };
  nlohmann::json wrongType = {
    {"currGrowth",{
      {"height", "3.2"},
      {"age", 4}
      }},
    {"currRes",{
      {"resources",  9999.0},
      {"biomass", 200.0}
      }
    },
    {"species", "testSpecies"}
    };
  nlohmann::json negativeHeight = {
    {"currGrowth",{
      {"height", -3.2},
      {"age", 4}
      }},
    {"currRes",{
      {"resources",  9999.0},
      {"biomass", 200.0}
      }
    },
    {"species", "testSpecies"}
    };
  nlohmann::json negativeAge = {
    {"currGrowth",{
      {"height", 3.2},
      {"age", -4}
      }},
    {"currRes",{
      {"resources",  9999.0},
      {"biomass", 200.0}
      }
    },
    {"species", "testSpecies"}
    };
  nlohmann::json negativeResources = {
    {"currGrowth",{
      {"height", 3.2},
      {"age", 4}
      }},
    {"currRes",{
      {"resources",  -9999.0},
      {"biomass", 200.0}
      }
    },
    {"species", "testSpecies"}
    };
  nlohmann::json negativeBiomass = {
    {"currGrowth",{
      {"height", 3.2},
      {"age", 4}
      }},
    {"currRes",{
      {"resources",  9999.0},
      {"biomass", -200.0}
      }
    },
    {"species", "testSpecies"}
    };
  nlohmann::json tooMuchBiomass = {
    {"currGrowth",{
      {"height", 3.2},
      {"age", 4}
      }},
    {"currRes",{
      {"resources",  9999.0},
      {"biomass", 1001.0}
      }
    },
    {"species", "testSpecies"}
    };
    nlohmann::json LifehistMissing = {
    {"currRes",{
      {"resources",  9999.0},
      {"biomass", 1001.0}
      }
    },
    {"species", "testSpecies"}
    };
    nlohmann::json resAllocMissing = {
    {"currGrowth",{
      {"height", 3.2},
      {"age", 4}
      }
    },
    {"species", "testSpecies"}
    };
  EXPECT_NO_THROW(Individual::create(traits, soil, correctVariables));
  std::unique_ptr<Individual> customIndividual = Individual::create(traits, soil, correctVariables);
    EXPECT_EQ(customIndividual->getArea(0,10), 100.0); //biomass / density
    //rounding error due to float- int conversion
  std::pair<bool, int> result = customIndividual->age();
  EXPECT_TRUE(result.first);

  ASSERT_ANY_THROW(Individual::create(traits, soil, heightMissing));
  ASSERT_ANY_THROW(Individual::create(traits, soil, ageMissing));
  ASSERT_ANY_THROW(Individual::create(traits, soil, resourcesMissing));
  ASSERT_ANY_THROW(Individual::create(traits, soil, biomassMissing));
  ASSERT_ANY_THROW(Individual::create(traits, soil, wrongType));
  ASSERT_ANY_THROW(Individual::create(traits, soil, negativeHeight));
  ASSERT_ANY_THROW(Individual::create(traits, soil, negativeAge));
  ASSERT_ANY_THROW(Individual::create(traits, soil, negativeResources));
  ASSERT_ANY_THROW(Individual::create(traits, soil, negativeBiomass));
  ASSERT_ANY_THROW(Individual::create(traits, soil, tooMuchBiomass));
  ASSERT_ANY_THROW(Individual::create(traits, soil, LifehistMissing));
  ASSERT_ANY_THROW(Individual::create(traits, soil, resAllocMissing));
}

TEST_F(IndividualTest, age){
  EXPECT_EQ(i->getArea(0.0,10.0), 50.0);
  std::pair<bool, int> results = i->age();
  EXPECT_EQ(results.first, false); //death by maintenance costs
  EXPECT_EQ(results.second, 0);
  i->feed(120);
  results = i->age();
  EXPECT_EQ(results.first, true);
  EXPECT_EQ(results.second, 1);
}

TEST_F(IndividualTest, getArea){
  i->age();
  i->feed(120);
  for (int j = 0; j < 11; j++){ //plant needs a while to grow to more than zero height
    i->age();
  }
  EXPECT_FLOAT_EQ(i->getArea(0,10), 82.25);
  EXPECT_FLOAT_EQ(i->getArea(0,7.046377671), 82.25); //actual height after 11 time steps
  EXPECT_FLOAT_EQ(i->getArea(0,4), 82.25* 4/7.046377671); 
  EXPECT_FLOAT_EQ(i->getArea(13,17), 0.0);
  EXPECT_FLOAT_EQ(i->getArea(2,2), 0.0);
}

TEST_F(IndividualTest, feedValidSoil){
  i->feed(21);
  std::pair<bool, int> results = i->age();
  EXPECT_EQ(results.first, true);
  i->feed(0);
  results = i->age();
  EXPECT_EQ(results.first, false);
  i->feed(10000000);
  results = i->age();
  EXPECT_EQ(results.first, true);
  EXPECT_EQ(results.second, 99999);
  for (int j = 0; j < 15; j++){
    i->age();
  }
  results = i->age();
  EXPECT_EQ(results.first, true);
}

TEST_F(IndividualTest, feedInvalidSoil){
  i->feed(21);
  std::pair<bool, int> results = i->age();
  EXPECT_EQ(results.first, true);
  soil->m_name = "clay";
  i->feed(10000000);
  results = i->age();
  EXPECT_EQ(results.first, false);
}