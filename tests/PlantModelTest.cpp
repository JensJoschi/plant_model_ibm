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
        el::Loggers::getLogger("PLANTS");
        el::Loggers::reconfigureAllLoggers(conf); 
        el::Loggers::addFlag(el::LoggingFlag::MultiLoggerSupport);

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
    bool condition = (results_grass.at("(1, 0, 1)") == 68.0/10000 || results_grass.at("(1, 0, 1)") == 88.0/10000);
    EXPECT_TRUE(condition) << "Initial size is not as expected ("<< results_grass.at("(1, 0, 1)") << "). Mersenne Twister should work on any platform, but FuncGroup calls std::uniform_int_distribution, whose implementation is platform-specific.";
}


TEST_F(PlantModelTest, InitializeOK){
    Landscape<double> initial =  p->getPFGabund(0);
    p->initialize(1);
    Landscape<double> results_grass =  p->getPFGabund(0);
    EXPECT_EQ(results_grass.at("(1, 0, 1)"),initial.at("(1, 0, 1)")+10.0/10000); //10 seeds from seedpool
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
    EXPECT_TRUE(std::abs(results_grass.at("(0, 0, 0)") - 0.1568) < 0.0001 || std::abs(results_grass.at("(0, 0, 0)") - 0.2101) < 0.0001);
    EXPECT_TRUE(std::abs(results_grass.at("(0, 0, 1)") - 0.1966) < 0.0001 || std::abs(results_grass.at("(0, 0, 1)") - 0.2529) < 0.0001);
    EXPECT_TRUE(std::abs(results_grass.at("(0, 1, 1)") - 0.1829) < 0.0001 || std::abs(results_grass.at("(0, 1, 1)") - 0.2377) < 0.0001);
    EXPECT_TRUE(std::abs(results_grass.at("(0, 2, 1)") - 0.1935) < 0.0001 || std::abs(results_grass.at("(0, 2, 1)") - 0.2392) < 0.0001);
    EXPECT_TRUE(std::abs(results_grass.at("(1, 0, 1)") - 0.1125) < 0.0001 || std::abs(results_grass.at("(1, 0, 1)") - 0.2467) < 0.0001);
    EXPECT_TRUE(std::abs(results_grass.at("(1, 1, 0)") - 0.0961) < 0.0001 || std::abs(results_grass.at("(1, 1, 0)") - 0.2528) < 0.0001);
    EXPECT_TRUE(std::abs(results_grass.at("(1, 2, 0)") - 0.116) < 0.0001 || std::abs(results_grass.at("(1, 2, 0)") - 0.2531) < 0.0001);
    EXPECT_FLOAT_EQ(results_grass.at("(2, 0, 0)"), 0.0);
    EXPECT_FLOAT_EQ(results_grass.at("(2, 2, 0)"), 0.0);
}
