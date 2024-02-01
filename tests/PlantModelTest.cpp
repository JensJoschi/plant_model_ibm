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
 // --------------------------------------------------------------------------


#include "p_PlantModel.h"
#include "gtest/gtest.h"
#include "rng.h"

// This file includes unit tests for the class PlantModel. See tests/CMake file 
// and google test documentation.

class PlantModelTest : public testing::Test {
    protected:
    PlantModel* p;

    void SetUp() override {
        std::filesystem::current_path();

        el::Configurations conf("debuglog.conf"); //silences the loggers
        el::Loggers::reconfigureAllLoggers(conf); 

        RNGs::mersenne = std::mt19937{2230};
        p = new PlantModel("test.json");
    }

//     void TearDownTestSuite() {
//     delete p;
//     p = nullptr;
//   }
};

TEST_F(PlantModelTest, buildOK){
    Landscape<double> results_grass =  p->getPFGabund(0);
    bool condition = (results_grass.at("(1, 0, 1)") == 68.0 || results_grass.at("(1, 0, 1)") == 88.0);
    EXPECT_TRUE(condition) << "Initial size is not as expected ("<< results_grass.at("(1, 0, 1)") << "). Mersenne Twister should work on any platform, but FuncGroup calls std::uniform_int_distribution, whose implementation is platform-specific.";
}


TEST_F(PlantModelTest, InitializeOK){
    Landscape<double> initial =  p->getPFGabund(0);
    p->initialize(1);
    Landscape<double> results_grass =  p->getPFGabund(0);
    EXPECT_EQ(results_grass.at("(1, 0, 1)"),initial.at("(1, 0, 1)")*0.5+10.0); //10 seeds from seedpool; 50% died of fire
}

TEST_F(PlantModelTest, ExampleRun){
    p->initialize(1);
    std::vector<std::string> keys{"(0, 0, 0)", "(0, 0, 1)", "(0, 1, 0)", 
                                "(0, 1, 1)", "(0, 2, 0)", "(0, 2, 1)",
                                "(1, 0, 0)", "(1, 0, 1)", "(1, 1, 0)",
                                "(1, 1, 1)", "(1, 2, 0)", "(1, 2, 1)",
                                "(2, 0, 0)", "(2, 0, 1)", "(2, 1, 0)",
                                "(2, 1, 1)", "(2, 2, 0)", "(2, 2, 1)"};
    Landscape<std::map<std::string, double>> testdisturbance(keys);
    std::map<std::string, double> filler{{"fire", 0.0}};
    std::map<std::string, double> filler2{{"fire", 0.2}};
    testdisturbance.fillWith(filler);
    testdisturbance.setValue("(0, 0, 0)", filler2);
    Landscape<std::string> validSoils(keys);
    validSoils.fillWith("SaLo_l_2");
    p->createInputMaps(validSoils, testdisturbance, true);
    for (int i  = 0; i< 2; i++){
        p->TPlusOne_JJ();
    }
    Landscape<double> results_grass =  p->getPFGabund(0);
    
    EXPECT_TRUE(std::abs(results_grass.at("(0, 0, 0)") - 946) < 0.0001 || std::abs(results_grass.at("(0, 0, 0)") - 1070) < 0.0001);
    EXPECT_TRUE(std::abs(results_grass.at("(0, 0, 1)") - 1053) < 0.0001 || std::abs(results_grass.at("(0, 0, 1)") - 1296) < 0.0001);
    EXPECT_TRUE(std::abs(results_grass.at("(0, 1, 1)") - 1003) < 0.0001 || std::abs(results_grass.at("(0, 1, 1)") - 1265) < 0.0001);
    EXPECT_TRUE(std::abs(results_grass.at("(0, 2, 1)") - 1041) < 0.0001 || std::abs(results_grass.at("(0, 2, 1)") - 1152) < 0.0001);
    EXPECT_TRUE(std::abs(results_grass.at("(1, 0, 1)") - 626) < 0.0001 || std::abs(results_grass.at("(1, 0, 1)") - 1252) < 0.0001);
    EXPECT_TRUE(std::abs(results_grass.at("(1, 1, 0)") - 966) < 0.0001 || std::abs(results_grass.at("(1, 1, 0)") - 1314) < 0.0001);
    EXPECT_TRUE(std::abs(results_grass.at("(1, 2, 0)") - 1118) < 0.0001 || std::abs(results_grass.at("(1, 2, 0)") - 1342) < 0.0001);
    EXPECT_FLOAT_EQ(results_grass.at("(2, 0, 0)"), 0.0);
    EXPECT_FLOAT_EQ(results_grass.at("(2, 2, 0)"), 0.0);
}

TEST_F(PlantModelTest, savePFG){
    p->initialize(1);
    Landscape<double> expected =  p->getPFGabund(1);
    p->savePFG("generalist_tree");
    std::ifstream i("RESULTS/generalist_tree.json");
    nlohmann::json j = nlohmann::json::parse(i);
    EXPECT_EQ(j.at("(0, 1, 1)"), expected.at("(0, 1, 1)"));
    std::remove("RESULTS/generalist_tree.json");

    p->savePFG("generalist_tree", "foo");
    std::ifstream i2("RESULTS/foo");
    nlohmann::json j2 = nlohmann::json::parse(i2);
    EXPECT_EQ(j2.at("(0, 1, 1)"), expected.at("(0, 1, 1)"));
    std::remove("RESULTS/foo");
}

TEST_F(PlantModelTest, savePFG_year){
    p->initialize(1);
    Landscape<double> expected =  p->getPFGabund(1);
    p->savePFG("generalist_tree", 1);
    std::ifstream i("RESULTS/generalist_tree_1.json");
    nlohmann::json j = nlohmann::json::parse(i);
    EXPECT_EQ(j.at("(0, 1, 1)"), expected.at("(0, 1, 1)"));
    std::remove("RESULTS/generalist_tree_1.json");

    p->savePFG("generalist_tree", 1, "goo" );
    std::ifstream i2("RESULTS/goo");
    nlohmann::json j2 = nlohmann::json::parse(i2);
    EXPECT_EQ(j2.at("(0, 1, 1)"), expected.at("(0, 1, 1)"));
    std::remove("RESULTS/goo");
}

TEST_F(PlantModelTest, saveAll){
    p->initialize(1);
    Landscape<double> expectedG =  p->getPFGabund(0);
    Landscape<double> expectedT =  p->getPFGabund(1);
    p->saveAll();
    std::ifstream i("RESULTS/biomass.json");
    nlohmann::json j = nlohmann::json::parse(i);
    EXPECT_EQ(j.at("(0, 1, 1)").at("generalist_grass"), expectedG.at("(0, 1, 1)"));
    EXPECT_EQ(j.at("(0, 1, 1)").at("generalist_tree"), expectedT.at("(0, 1, 1)"));
    std::remove ("RESULTS/biomass.json");

    p->saveAll("loo");
    std::ifstream i2("RESULTS/loo");
    nlohmann::json j2 = nlohmann::json::parse(i2);
    EXPECT_EQ(j2.at("(0, 1, 1)").at("generalist_grass"), expectedG.at("(0, 1, 1)"));
    EXPECT_EQ(j2.at("(0, 1, 1)").at("generalist_tree"), expectedT.at("(0, 1, 1)"));
    std::remove ("RESULTS/loo");
}