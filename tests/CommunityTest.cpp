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

// This file includes unit tests for the class Community. See tests/CMake file 
// and google test documentation.

#include "Community.h"
#include "Traits.h"
#include "Soil.h"
#include "Illumination.h"

/** @cond */
#include "gtest/gtest.h"
#include <memory> 
#include <map>
#include <string>
#include <nlohmann/json.hpp>
#include <vector>
/** @endcond */

class CommunityTest : public ::testing::Test {
    protected:
    std::unique_ptr<Community> c;
    static const Traits* traits;
    static const std::vector<Stratum>* strata;
    
    static void SetUpTestSuite() { //creates shared trait Definitions for all tests
       nlohmann::json j = {
        {"minDepth", 10},
        {"acceptedSoils", {{"sand", true}, {"clay", false}}},
        {"size", 1},

        {"conversionEfficiency", 1.0},
        {"maintenanceCosts", 0.1},
        {"seedAllocation", 0.01},
        {"biomassAllocation", 0.5},
        {"maxBiomass", 500},

        {"Dormancy", false},
        {"GerminationSuccess", 0.1},
        {"MortalityDormant", 0.05},
        {"DormancyBreakRate", 0.8},

        {"MaturationTime", 10},
        {"LifeSpan", 20},
        {"MaxHeight", 8},

        {"density", 2.0}

    };
    traits = new Traits(j);
    Stratum s1{0, 2, 100};
    Stratum s2{2, 4, 100};
    Stratum s3{4, 6, 100};
    strata = new std::vector<Stratum>{s1, s2, s3};

    }

  static void TearDownTestSuite() {
    delete traits;
    traits = nullptr;
  }
};

const Traits* CommunityTest::traits = nullptr;
const std::vector<Stratum>* CommunityTest::strata = nullptr;

TEST_F(CommunityTest, builds){
    std::map<std::string_view, const Traits*> traitlist;
    traitlist["species"] = traits;

    c = std::make_unique<Community>(10, 10 , "sand", traitlist, 10, 100, *strata, 1, 0.1);
    ASSERT_TRUE(c != nullptr);
    c.reset();
}

TEST_F(CommunityTest, buildsWithJson){
  nlohmann::json correctVariables = {
      {"height", 3.2},
      {"age", 4},
      {"resources", 9999.0},
      {"biomass", 200.0}};
  nlohmann::json j = {
        {"species", correctVariables},
    };

    std::map<std::string_view, const Traits*> traitlist;
    traitlist["species"] = traits;
    c = std::make_unique<Community>(10, 10 , "sand", traitlist, 10, 100, *strata,1,0.1, j);
    ASSERT_TRUE(c != nullptr);
    c.reset();
    // j["unknownSpecies"] = correctVariables;
    // EXPECT_THROW(c = std::make_unique<Community>(10, 10 , "sand", traitlist, 10, 100, *strata, j), std::invalid_argument);
}

// TEST_F(CommunityTest, getCount){
//   std::map<std::string_view, const Traits*> traitlist;
//   traitlist["species"] = traits;
//   traitlist["anotherSpecies"] = traits;
//   c = std::make_unique<Community>(10, 10 , "sand", traitlist, 10, 100);

//   ASSERT_TRUE(c->getCount() == 10);
//   ASSERT_TRUE(c->getCount("species") + c->getCount("anotherSpecies") == 10);
//   //ASSERT_TRUE(c->getCount("species") == 4); //requires fixed RNG
//   //ASSERT_TRUE(c->getCount("anotherSpecies") == 6);
//   ASSERT_TRUE(c->getCount("something else") == 0);
// }

// TEST_F(CommunityTest, startingConditions){
//     std::map<std::string_view, const Traits*> traitlist;
//     traitlist["species"] = traits;
//     c = std::make_unique<Community>(10, 10 , "sand", traitlist, 10, 100);
//     ASSERT_TRUE(c->getCount("species") == 10);
//     c = std::make_unique<Community>(3, 10 , "sand", traitlist, 10, 100);
//     ASSERT_TRUE(c->getCount("species") == 3);
//     c = std::make_unique<Community>(10, 10 , "sand", traitlist, 2, 100);
//     ASSERT_TRUE(c->getCount("species") == 2);
//     c = std::make_unique<Community>(10, 10 , "sand", traitlist, 10, 0);
//     ASSERT_TRUE(c->getCount("species") == 10);

//     c = std::make_unique<Community>(10, 10 , "clay", traitlist, 10, 100);
//     ASSERT_TRUE(c->getCount("species") == 0);

//     //assertion failures:
//     // c = std::make_unique<Community>(10, 10 , "foo", traitlist, 10, 100);
//     //    std::map<std::string_view, const Traits*> empty;
//     //  c = std::make_unique<Community>(10, 10 , "sand", empty, 10, 100); 
// }


// TEST_F(CommunityTest, rainSeeds){
//     std::map<std::string_view, const Traits*> traitlist;
//     traitlist["species"] = traits;
//     traitlist["anotherSpecies"] = traits;
//     c = std::make_unique<Community>(10, 10 , "sand", traitlist, 10, 100);
//     int ageResult1 = c->m_seeds.at("species")->age(0);
//     EXPECT_EQ(ageResult1, 10); //10% germination rate according to traits
//     int ageResult2 = c->m_seeds.at("anotherSpecies")->age(0);
//     EXPECT_EQ(ageResult2, 10); 
// //actual test:
//     c.reset();
//     c = std::make_unique<Community>(10, 10 , "sand", traitlist, 10, 100);
//     std::map<std::string_view, int> seeds;
//     seeds["species"] = 100;
//     c->rainSeeds(seeds);
//     int ageResult3 = c->m_seeds.at("species")->age(0);
//     EXPECT_EQ(ageResult3, 20);
//     int ageResult4 = c->m_seeds.at("anotherSpecies")->age(0);
//     EXPECT_EQ(ageResult4, ageResult2);
//     seeds["unrecognized Species"] = 3;
//     EXPECT_THROW(c->rainSeeds(seeds), std::out_of_range);
// }

// TEST_F(CommunityTest, provideLight){
//     std::map<std::string_view, const Traits*> traitlist;
//     traitlist["species"] = traits;
//     traitlist["anotherSpecies"] = traits;
//     c = std::make_unique<Community>(10, 10 , "sand", traitlist, 10, 100);
//     int result = c->provideResources(0, 10, 100, 1000);
//     EXPECT_EQ(result, 50); //area of each individual is height*biomass/density = 1*100/2 = 50; so they cover half the illumArea
//     result = c->provideResources(0, 10, 100, 40, false);
//     EXPECT_EQ(result, 0); //now they cover more than the illumArea
//     //note: this test does not check if the light is actually used by the individuals
//     //Doing so would require adding this test as friend to the Community class, and possibly to Individual as well
// }

// TEST_F(CommunityTest, ageNoResources){
//     std::map<std::string_view, const Traits*> traitlist;
//     traitlist["species"] = traits;
//     c = std::make_unique<Community>(10, 10 , "sand", traitlist, 1, 0);
//     std::map<std::string, int> seeds = c->age();
//     EXPECT_EQ(seeds.size(), 1);
//     EXPECT_EQ(seeds.at("species"), 0);
//     EXPECT_EQ(c->getCount(0, 10, "species"), 0);

//     c = std::make_unique<Community>(100, 10 , "sand", traitlist, 1, 90); //expectation: new seeds = 90 * germination rate
//     seeds = c->age();
//     EXPECT_EQ(seeds.size(), 1);
//     EXPECT_EQ(seeds.at("species"), 0);
//     EXPECT_EQ(c->getCount(0, 10, "species"), 9); 

//     c = std::make_unique<Community>(8, 10 , "sand", traitlist, 1, 100); // expectation: new seeds  = capacity
//     seeds = c->age();
//     EXPECT_EQ(seeds.size(), 1);
//     EXPECT_EQ(seeds.at("species"), 0);
//     EXPECT_EQ(c->getCount(0, 10, "species"), 8);
// }

// TEST_F(CommunityTest, ageWithResources){
//     std::map<std::string_view, const Traits*> traitlist;
//     traitlist["species"] = traits;
//     c = std::make_unique<Community>(10, 10 , "sand", traitlist, 1, 0);
//     c->provideResources(0, 10, 2000, 100);
//     std::map<std::string, int> seeds = c->age();
//     EXPECT_TRUE(seeds.at("species") > 0);
//     EXPECT_EQ(c->getCount(0, 10, "species"), 1);

//     for (int i = 0; i < 19; i++) {
//         c->provideResources(0, 10, 10000, 1000, false);
//         std::map<std::string, int> newSeeds = c->age();
//         EXPECT_TRUE(newSeeds.at("species") > 0); 
//         EXPECT_EQ(c->getCount(0, 10, "species"), 1); //new seeds are not placed back into community
//     }
//     c->provideResources(0, 10, 10000, 1000);
//     c->age(); //Lifespan exceeded
//     EXPECT_EQ(c->getCount(0, 10, "species"), 0); 
// }


// TEST_F(CommunityTest, ageWithResourceAndSeedRain){
//     std::map<std::string_view, const Traits*> traitlist;
//     traitlist["species"] = traits;
//     c = std::make_unique<Community>(30, 10 , "sand", traitlist, 1, 0);
//     std::map<std::string, int> newSeeds;
//     EXPECT_EQ(c->getCount(), 1);
//     for (int i = 0; i < 100; i++) {
//         c->provideResources(0, 10, 2000, 1000, false);
//         newSeeds = c->age();
//         std::map<std::string_view, int> seedsView;
//         for (const auto& pair : newSeeds) {seedsView.emplace(pair.first, pair.second);}
//         c->rainSeeds(seedsView);
//     }
//     EXPECT_EQ(c->getCount(), 30);
// }