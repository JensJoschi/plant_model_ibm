
#include "PlantModel.h"
#include "rng.h"

/** @cond */
#include "gtest/gtest.h"
/** @endcond */

class NeighbourTest : public testing::Test {
protected:
    static nlohmann::json config;
    static nlohmann::json data;
    static nlohmann::json traits;
    static void SetUpTestSuite() {
	nlohmann::json ind ={
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
		{"name", "Name"}
	};
	traits = {
        {"Name", ind}
    };

        config =
        {
            {"SimulDuration", 5},
            {"SaveYears", std::vector{1,2,4}},
            {"NoStrata", 4},
            {"VoxelArea", 70},
            {"VoxelHeight", 100},
            {"StartingPopSize", 1},
            {"InitialSeeds", 0},
            {"SoilCapacity", 10},
            {"SeedRain",0},
            {"LightDistributionFactor", 1},
            {"Diffusion", 0.0},
            {"DoesDispersal", false},
            {"DoesSoilClass", true}, //unused
            {"DoesSoilDepth", true}, //unused
            {"DoesRegionalModel", false} //unused?
        };
        data = {
            {"InputDir", "IN/"},
            {"SaveDir", "RESULTS/"},
            {"MaskFile", "lightN.json"},
            {"LightFile", "lightN.json"},
            {"DepthFile", "depthN.json"},
            {"SoilClassFile", "soilclassN.json"},
            {"PlantLogger", "debuglog.conf"}
        };
    }

    void SetUp() override {
        el::Configurations conf("debuglog.conf"); //silences the loggers
        el::Loggers::reconfigureAllLoggers(conf); 
    }
};
nlohmann::json NeighbourTest::config = nlohmann::json::array();
nlohmann::json NeighbourTest::data = nlohmann::json::array();
nlohmann::json NeighbourTest::traits = nlohmann::json::array();

TEST_F(NeighbourTest, builds){
    PlantModel p(config, data, traits);
}

TEST_F(NeighbourTest, works){
    PlantModel p(config, data, traits);
    p.TPlusOne();
    Landscape<float> res = p.getArea();
    EXPECT_TRUE(res.at({0,0,0}) > 0);
    EXPECT_TRUE(res.at({0,1,0}) == 0); //takes a while to grow into neighbour.
    Landscape<int> res2 = p.getNumber("Name");
    EXPECT_TRUE(res2.at({0,0,0}) >0);
    EXPECT_TRUE(res2.at({0,1,0}) == 0);

    for (int i = 0; i < 3; i++){ 
        p.TPlusOne();
    }
    res = p.getArea();
    EXPECT_TRUE(res.at({0,0,0}) > 0);
    EXPECT_TRUE(res.at({0,1,0}) > 0);
    res2 = p.getNumber("Name");
    EXPECT_TRUE(res2.at({0,0,0}) >0);
    EXPECT_TRUE(res2.at({0,1,0}) == 0);
}
