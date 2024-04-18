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

// This file includes unit tests for the class Illumination. See tests/CMake file 
// and google test documentation.

#include "Illumination.h"



/** @cond */
#include "gtest/gtest.h"
#include <memory> 
/** @endcond */

class IlluminationTest : public ::testing::Test {
    protected:
    std::vector<Stratum> strata;
    static const Traits* traits;
    static std::shared_ptr<Soil> soil;

    void SetUp() override {
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
        soil = std::make_shared<Soil>(4,10,"sand");

        strata.push_back({0, 1, 150, 0});
        strata.push_back({1, 2, 150, 0});
        strata.push_back({2, 3, 150, 0});
        strata.push_back({3, 4, 150, 0});
    }
    static void TearDownTestSuite() {
    delete traits;
    traits = nullptr;
  }
};

const Traits* IlluminationTest::traits = nullptr;
std::shared_ptr<Soil> IlluminationTest::soil = nullptr;


TEST_F(IlluminationTest, smallPlantNoShade){//stratum 0 only
    std::unique_ptr<Individual> i = Individual::create(traits, soil); 
    EXPECT_TRUE (i);
    std::multimap<std::string_view, std::unique_ptr<Individual>> individuals;
    individuals.insert({"test", std::move(i)});
    EXPECT_TRUE (individuals.find("test")->second);
    Illumination illu {individuals, strata};
    std::pair<bool,int> result = individuals.find("test")->second->age();
    EXPECT_FALSE(result.first);
    illu.sendLightBeam(0);
    result = individuals.find("test")->second->age();
    EXPECT_FALSE(result.first);
    illu.sendLightBeam(100);
    result = individuals.find("test")->second->age();
    EXPECT_TRUE(result.first);
    }

TEST_F(IlluminationTest, largePlantNoShade){
    std::unique_ptr<Individual> i = Individual::create(traits, soil); 
    std::pair<bool,int> result;
    for (int j = 0; j < 11; j++){
        i->feed(20);
        result = i->age();
        EXPECT_TRUE(result.first);
    }
    result = i->age();
    EXPECT_FALSE(result.first);
    EXPECT_TRUE (i);
    std::multimap<std::string_view, std::unique_ptr<Individual>> individuals;
    individuals.insert({"test", std::move(i)});
    EXPECT_TRUE (individuals.find("test")->second);
    Illumination illu {individuals, strata};
    result = individuals.find("test")->second->age();
    EXPECT_FALSE(result.first);
    illu.sendLightBeam(1000);
    result = individuals.find("test")->second->age();
    EXPECT_TRUE(result.first);
}

//Testing is very cumbersome, as the height and resource stored by individuals cannot be controlled
//Implement Individual(json j) first. 
TEST_F(IlluminationTest, excessivelyLargePlantNoShade){}  //grows larger than the community/Voxel size
TEST_F(IlluminationTest, onePlantShadesAnother){}
TEST_F(IlluminationTest, TwoPlantsSameStratum){}

TEST_F(IlluminationTest, largeCommunityNoShade){}
TEST_F(IlluminationTest, IllumMedShade){}
TEST_F(IlluminationTest, IllumFullShade){}

