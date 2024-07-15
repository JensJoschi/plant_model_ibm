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

// This file includes unit tests for the class PlantModel. See tests/CMake file 
// and google test documentation.

#include "PlantModel.h"
#include "rng.h"

/** @cond */
#include "gtest/gtest.h"
/** @endcond */

class PlantModelTest : public testing::Test {
protected:
    static nlohmann::json config;
    static nlohmann::json data;
    static nlohmann::json traits;
    static void SetUpTestSuite() {
    nlohmann::json shallowSand ={
		{"minDepth", 5},
		{"acceptedSoils", {{"sand", true}, {"clay", false}, {"no_soil", false}}},
		{"size", 1},

		{"conversionEfficiency", 1.0},
		{"maintenanceCosts", 0.1},
		{"seedAllocation", 0.01},
		{"biomassAllocation", 0.5},
		{"maxBiomass", 800},

		{"Dormancy", false},
		{"GerminationSuccess", 0.1},
		{"MortalityDormant", 0.05},
		{"DormancyBreakRate", 0.8},

		{"MaturationTime", 10},
		{"LifeSpan", 20},
		{"MaxHeight", 40},

		{"density", 2.0},
		{"name", "shallowSand"}
	};
	nlohmann::json deepSand ={
		{"minDepth", 15},
		{"acceptedSoils", {{"sand", true}, {"clay", false}, {"no_soil", false}}},
		{"size", 1},

		{"conversionEfficiency", 1.0},
		{"maintenanceCosts", 0.1},
		{"seedAllocation", 0.01},
		{"biomassAllocation", 0.5},
		{"maxBiomass", 800},

		{"Dormancy", false},
		{"GerminationSuccess", 0.1},
		{"MortalityDormant", 0.05},
		{"DormancyBreakRate", 0.8},

		{"MaturationTime", 10},
		{"LifeSpan", 20},
		{"MaxHeight", 40},

		{"density", 2.0},
		{"name", "deepSand"}
	};
	nlohmann::json shallowClay ={
		{"minDepth", 5},
		{"acceptedSoils", {{"sand", false}, {"clay", true}, {"no_soil", false}}},
		{"size", 1},

		{"conversionEfficiency", 1.0},
		{"maintenanceCosts", 0.1},
		{"seedAllocation", 0.01},
		{"biomassAllocation", 0.5},
		{"maxBiomass", 800},

		{"Dormancy", false},
		{"GerminationSuccess", 0.1},
		{"MortalityDormant", 0.05},
		{"DormancyBreakRate", 0.8},

		{"MaturationTime", 10},
		{"LifeSpan", 20},
		{"MaxHeight", 40},

		{"density", 2.0},
		{"name", "shallowClay"}
	};
	nlohmann::json deepClay ={
		{"minDepth", 15},
		{"acceptedSoils", {{"sand", false}, {"clay", true}, {"no_soil", false}}},
		{"size", 1},

		{"conversionEfficiency", 1.0},
		{"maintenanceCosts", 0.1},
		{"seedAllocation", 0.01},
		{"biomassAllocation", 0.5},
		{"maxBiomass", 800},

		{"Dormancy", false},
		{"GerminationSuccess", 0.1},
		{"MortalityDormant", 0.05},
		{"DormancyBreakRate", 0.8},

		{"MaturationTime", 10},
		{"LifeSpan", 20},
		{"MaxHeight", 40},

		{"density", 2.0},
		{"name", "deepClay"}
	};
	traits = {
		{"shallowSand", shallowSand},
		{"deepSand", deepSand},
		{"shallowClay", shallowClay},
		{"deepClay", deepClay}
	};

        config =
        {
            {"SimulDuration", 5},
            {"SaveYears", std::vector{1,2,4}},
            {"NoStrata", 4},
            {"VoxelArea", 100},
            {"VoxelHeight", 100},
            {"StartingPopSize", 10},
            {"InitialSeeds", 0},
            {"SoilCapacity", 100},
            {"SeedRain",0},
            {"LightDistributionFactor", 10},
            {"Diffusion", 0.5},
            {"DoesDispersal", true}, //unused
            {"DoesSoilClass", true}, //unused
            {"DoesSoilDepth", true}, //unused
            {"DoesRegionalModel", false} //unused?
        };
        data = {
            {"InputDir", "IN/"},
            {"SaveDir", "RESULTS/"},
            {"MaskFile", "light.json"},
            {"LightFile", "light.json"},
            {"DepthFile", "depth.json"},
            {"SoilClassFile", "soilclass.json"},
            // {"RegionalModelFile","regionalmodel.json"},
            // {"ManagementFile", "management.json"},
            {"PlantLogger", "debuglog.conf"}
        };
    }

    void SetUp() override {
        el::Configurations conf("debuglog.conf"); //silences the loggers
        el::Loggers::reconfigureAllLoggers(conf); 
    }
};
nlohmann::json PlantModelTest::config = nlohmann::json::array();
nlohmann::json PlantModelTest::data = nlohmann::json::array();
nlohmann::json PlantModelTest::traits = nlohmann::json::array();

TEST_F(PlantModelTest, builds){
    PlantModel p(config, data, traits);
}

TEST_F(PlantModelTest, getters){
	//tests that all the getter functions return correct result.
	//coordinate (0,0,0) allows growth of shallowSand, (0,2,0) does not.
	//deepSand can live on (0,1,0) but not (0,0,0)
	//would be easier if we could use a PlantModel::PlantModel(config, data, traits, json population)
    PlantModel p(config, data, traits);
    Landscape<int> r =  p.getNumber("shallowSand");
	EXPECT_TRUE(r.at(Coordinates{0,0,0}) >0 && r.at(Coordinates{0,0,0}) <= 10);
	EXPECT_EQ(r.at(Coordinates{0,2,0}), 0);

	Landscape<std::map<std::string, int>> n =  p.getNumber();
	EXPECT_TRUE(n.at(Coordinates{0,0,0}).at("shallowSand") > 0 && n.at(Coordinates{0,0,0}).at("shallowSand") <= 10);
	EXPECT_EQ(n.at(Coordinates{0,2,0}).at("shallowSand"), 0);
	EXPECT_TRUE(n.at(Coordinates{0,0,0}).at("deepSand") == 0);
	EXPECT_TRUE(n.at(Coordinates{0,1,0}).at("deepSand") > 0);
	//might fail on some OS due to RNG. Behavior on different OS caused by different std::uniform implementations
	Landscape<std::map<std::string, int>> n2 =  p.getNumber(0, 25);
	EXPECT_TRUE(n2.at(Coordinates{0,0,0}).at("shallowSand") > 0 && n2.at(Coordinates{0,0,0}).at("shallowSand") <= 10);
	EXPECT_EQ(n2.at(Coordinates{0,2,0}).at("shallowSand"), 0);
	EXPECT_TRUE(n2.at(Coordinates{0,0,0}).at("deepSand") == 0);
	EXPECT_TRUE(n2.at(Coordinates{0,1,0}).at("deepSand") > 0);
	n2 = p.getNumber(41, 100);
	EXPECT_EQ(n2.at(Coordinates{0,0,0}).at("shallowSand"), 0); //max height == 40

	Landscape<float> a =  p.getArea();
	EXPECT_EQ(a.at(Coordinates{0,0,0}), 0); //initial size is 0, plants first need to grow

	p.TPlusOne();
	a =  p.getArea();
	EXPECT_TRUE(a.at(Coordinates{0,0,0}) > 0 && a.at(Coordinates{0,0,0}) <= 200);

	Landscape<std::map<std::string, float>> b =  p.getBiomass();
	EXPECT_TRUE(b.at(Coordinates{0,0,0}).at("shallowSand") > 0 && b.at(Coordinates{0,0,0}).at("shallowSand") <= 800);
	EXPECT_TRUE(b.at(Coordinates{0,1,0}).at("deepSand") > 0 && b.at(Coordinates{0,1,0}).at("deepSand") <= 800);
}

TEST_F(PlantModelTest, TplusOneNoDisp){
	//tests that all plants die out after exceeding lifespan when no new seeds are added
	config["DoesDispersal"] = false;
	PlantModel p(config, data, traits);
	for (int i = 0; i < 20; i++){
		p.TPlusOne();
	}
	Landscape<int> r =  p.getNumber("shallowSand");
	EXPECT_TRUE(r.at(Coordinates{0,0,0}) >0);
	p.TPlusOne();
	r =  p.getNumber("shallowSand");
	EXPECT_EQ(r.at(Coordinates{0,0,0}), 0);
	config["DoesDispersal"] = true;
}

TEST_F(PlantModelTest, TplusOneWithDisp){
	//tests that population survives due to production of offspring
	PlantModel p(config, data, traits); //no annual seed input here
	for (int i = 0; i < 20; i++){
		p.TPlusOne();
	}
	Landscape<int> r =  p.getNumber("shallowSand");
	EXPECT_TRUE(r.at(Coordinates{0,0,0}) >0);
	p.TPlusOne();
	r =  p.getNumber("shallowSand");
	EXPECT_TRUE(r.at(Coordinates{0,0,0})>0);
}

TEST_F(PlantModelTest, Habsuit){
	//checks that soil depth and soil class prevent growth of plants, according to traits
	PlantModel p(config, data, traits);
	for (int i = 0; i < 10; i++){
		p.TPlusOne();
	}
	Landscape<int> shalS =  p.getNumber("shallowSand");
	Landscape<int> deepS =  p.getNumber("deepSand");
	Landscape<int> shalC =  p.getNumber("shallowClay");
	Landscape<int> deepC =  p.getNumber("deepClay");
	EXPECT_TRUE (shalS.at(Coordinates{0,0,0}) > 0);
	EXPECT_TRUE (shalS.at(Coordinates{0,1,0}) > 0);
	EXPECT_FALSE(shalS.at(Coordinates{0,2,0}) > 0);
	EXPECT_FALSE(shalS.at(Coordinates{0,3,0}) > 0);

	EXPECT_FALSE(deepS.at(Coordinates{0,0,0}) > 0);
	// EXPECT_TRUE (deepS.at(Coordinates{0,1,0}) > 0); fails on some compilers due to RNG gods.
	EXPECT_FALSE(deepS.at(Coordinates{0,2,0}) > 0);
	EXPECT_FALSE(deepS.at(Coordinates{0,3,0}) > 0);

	EXPECT_FALSE(shalC.at(Coordinates{0,0,0}) > 0);
	EXPECT_FALSE(shalC.at(Coordinates{0,1,0}) > 0);
	EXPECT_TRUE (shalC.at(Coordinates{0,2,0}) > 0);
	EXPECT_TRUE (shalC.at(Coordinates{0,3,0}) > 0);

	EXPECT_FALSE(deepC.at(Coordinates{0,0,0}) > 0);
	EXPECT_FALSE(deepC.at(Coordinates{0,1,0}) > 0);
	EXPECT_FALSE(deepC.at(Coordinates{0,2,0}) > 0);
	EXPECT_TRUE (deepC.at(Coordinates{0,3,0}) > 0);
}

TEST_F(PlantModelTest, lightCompetition){
	//one plant grows larger than the other. 
	//where light is insufficient, noone can survive. 
	//where light is limited, only the superior survives. At least the inferior should be rare.
	//where light is abundant, the inferiors may also sometimes survive, but the superior is more abundant.
	//eventually, the inferior should die out. This does not work yet though, because immatures currently produce seeds. 
	//because plants do not outgrow a voxel, the neighbouring voxels should be empty.
	nlohmann::json inferior = traits["shallowSand"];
	inferior["name"] = "inferior";
	inferior["maxBiomass"] = 300;
	inferior["MaturationTime"] = 2;
	nlohmann::json superior = traits["shallowSand"];
	superior["name"] = "superior";
	superior.at("MaxHeight") = 80;
	superior["maxBiomass"] = 300;
	superior["MaturationTime"] = 2;
	nlohmann::json t = {
		{"inferior", inferior},
		{"superior", superior}
	};
	nlohmann::json c2 = config;
	c2.at("Diffusion") = 0.0;
	c2.at("DoesDispersal") = true;
	c2.at("VoxelArea") = 1000;
	c2.at("LightDistributionFactor") = 100;
	c2.at("SoilCapacity") = 20;
	nlohmann::json data2 = data;
	data2.at("LightFile") = "light2.json";


	PlantModel p(c2, data2, t);
	//initially each plant has biomass 100 = 50 area. it needs 10 energy to surive. 
	//10 energy/50 area = 200 energy per 1000 area. 
	for (int i = 0; i < 10; i++){
		p.TPlusOne();
	}
	Landscape<std::map<std::string, int>> res =  p.getNumber(5,100);
	EXPECT_TRUE(res.at(Coordinates{0,0,0}).at("superior") > 0  && res.at(Coordinates{0,0,0}).at("superior") <= 20);
	// EXPECT_TRUE(res.at(Coordinates{0,0,0}).at("inferior") < 3); //sometimes a small number can still survive. <3 works on appleclang but may 
	// fail on other compilers due to different RNG implementations

	EXPECT_TRUE(res.at(Coordinates{1,0,0}).at("inferior") == 0 && res.at(Coordinates{1,0,0}).at("superior") == 0);

	EXPECT_TRUE(res.at(Coordinates{2,0,0}).at("superior") >0 && res.at(Coordinates{2,0,0}).at("superior") <= 20);
	// EXPECT_TRUE(res.at(Coordinates{2,0,0}).at("superior") > res.at(Coordinates{2,0,0}).at("inferior")); also fails due to rng?

	EXPECT_TRUE(res.at(Coordinates{2,4,0}).at("superior") == 0);
	EXPECT_TRUE(p.getArea().at(Coordinates{2,4,0}) == 0);

//does not yet work because immatures produce seeds
	// for (int i = 0; i < 10000; i++){
	// 	p.TPlusOne();
	// }
	// res =  p.getNumber(5,100);
	// EXPECT_TRUE(res.at(Coordinates{2,0,0}).at("superior") > 0  && res.at(Coordinates{0,0,0}).at("superior") <= 20);
	// EXPECT_TRUE(res.at(Coordinates{2,0,0}).at("inferior") == 0);
}