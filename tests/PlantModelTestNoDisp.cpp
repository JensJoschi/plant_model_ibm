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

class PlantModelTestNoDisp : public testing::Test {
    protected:
    PlantModel* p;

    void SetUp() override {
        el::Configurations conf("debuglog.conf"); //silences the loggers
        el::Loggers::reconfigureAllLoggers(conf); 
        p = new PlantModel("test_noDisp.json", "debuglog.conf", true);
    }

};

TEST_F(PlantModelTestNoDisp, buildOK){
    Landscape<double> results_grass =  p->getPFGabund(0);
    bool condition = (results_grass.at("(1, 0, 1)") == 68.0 || results_grass.at("(1, 0, 1)") == 88.0);
    EXPECT_TRUE(condition) << "Initial size is not as expected ("<< results_grass.at("(1, 0, 1)") << "). Mersenne Twister should work on any platform, but FuncGroup calls std::uniform_int_distribution, whose implementation is platform-specific.";
}


TEST_F(PlantModelTestNoDisp, InitializeOK){
    Landscape<double> initial =  p->getPFGabund(0);
    p->initialize(1);
    Landscape<double> results_grass =  p->getPFGabund(0);
    EXPECT_EQ(results_grass.at("(1, 0, 1)"),initial.at("(1, 0, 1)")*0.5);// 50% died of fire
}

TEST_F(PlantModelTestNoDisp, ExampleRun){
    Landscape<double> initialGrass =  p->getPFGabund(0);
    Landscape<double> initialTree =  p->getPFGabund(1);
    p->initialize(1);
    for (int i  = 0; i< 2; i++){
        p->TPlusOne_JJ();
    }
    Landscape<double> results_grass =  p->getPFGabund(0);
    Landscape<double> results_tree =  p->getPFGabund(1);
    EXPECT_FLOAT_EQ(results_grass.at("(0, 0, 0)"), 0.0); //killed by 1.0 fire
    EXPECT_FLOAT_EQ(results_tree.at ("(0, 0, 0)"), initialTree.at ("(0, 0, 0)")); //not affected by fire, only older

    EXPECT_NEAR(results_grass.at("(0, 0, 1)"), initialGrass.at("(0, 0, 1)")/8, 2.0f); //halved in each step due to 0.5 fire
    EXPECT_FLOAT_EQ(results_tree.at ("(0, 0, 1)"), initialTree.at ("(0, 0, 1)")); 
    EXPECT_NEAR(results_grass.at("(0, 1, 1)"), initialGrass.at("(0, 1, 1)")/8, 2.0f); 
    EXPECT_FLOAT_EQ(results_tree.at ("(0, 1, 1)"), initialTree.at ("(0, 1, 1)")); 
    EXPECT_NEAR(results_grass.at("(0, 2, 0)"), initialGrass.at("(0, 2, 0)")/8, 2.0f);
    EXPECT_FLOAT_EQ(results_tree.at ("(0, 2, 0)"), initialTree.at ("(0, 2, 0)")); 
    EXPECT_FLOAT_EQ(results_grass.at("(2, 2, 0)"), 0.0);
    EXPECT_FLOAT_EQ(results_tree.at ("(2, 2, 0)"), initialTree.at ("(2, 2, 0)")); 
}

TEST_F(PlantModelTestNoDisp, testDisturbance){
    Landscape<double> initialGrass =  p->getPFGabund(0);
    Landscape<double> initialTree  =  p->getPFGabund(1);
    p->initialize(1);
    std::vector<std::string> keys{"(0, 0, 0)", "(0, 0, 1)", "(0, 1, 0)", 
                                "(0, 1, 1)", "(0, 2, 0)", "(0, 2, 1)",
                                "(1, 0, 0)", "(1, 0, 1)", "(1, 1, 0)",
                                "(1, 1, 1)", "(1, 2, 0)", "(1, 2, 1)",
                                "(2, 0, 0)", "(2, 0, 1)", "(2, 1, 0)",
                                "(2, 1, 1)", "(2, 2, 0)", "(2, 2, 1)"};
    Landscape<std::map<std::string, double>> testdisturbance(keys);
    std::map<std::string, double> frog{{"frog", 0.8}};
    std::map<std::string, double> noFrog{{"frog", 0.0}};
    std::map<std::string, double> alsoNoFrog{{"frooooog", 0.8}};
    testdisturbance.fillWith(noFrog);
    testdisturbance.setValue("(0, 0, 1)", frog);
    testdisturbance.setValue("(0, 1, 1)", alsoNoFrog);
    Landscape<std::string> validSoils(keys);
    validSoils.fillWith("SaLo_l_2");
    p->createInputMaps(validSoils, testdisturbance, true);
      for (int i  = 0; i< 2; i++){
        p->TPlusOne_JJ();
    }
    Landscape<double> results_grass =  p->getPFGabund(0);
    Landscape<double> results_tree =  p->getPFGabund(1);
    //these are same as exampleRun:
    EXPECT_FLOAT_EQ(results_grass.at("(0, 0, 0)"), 0.0); //killed by fire
    EXPECT_FLOAT_EQ(results_tree.at ("(0, 0, 0)"), initialTree.at ("(0, 0, 0)"));
    EXPECT_NEAR(results_grass.at("(0, 1, 1)"), initialGrass.at("(0, 1, 1)")/8, 2.0f); //not a frog (disturbance does not exist)
    EXPECT_FLOAT_EQ(results_tree.at ("(0, 1, 1)"), initialTree.at ("(0, 1, 1)"));
    EXPECT_NEAR(results_grass.at("(0, 2, 0)"), initialGrass.at("(0, 2, 0)")/8, 2.0f);
    EXPECT_FLOAT_EQ(results_tree.at ("(0, 2, 0)"), initialTree.at ("(0, 2, 0)")); 
    EXPECT_FLOAT_EQ(results_grass.at("(2, 2, 0)"), 0.0);
    EXPECT_FLOAT_EQ(results_tree.at ("(2, 2, 0)"), initialTree.at ("(2, 2, 0)")); 
    //only this is different:
    EXPECT_NEAR(results_grass.at("(0, 0, 1)"), initialGrass.at("(0, 0, 1)")*0.054872, 2.0f); //0.5 fire + 0.8 frog with 0.3 effect: 1 -(0.8*0.3) = 0.76; 0.5*0.76 = 0.38; 0.38^3 = 0.054872
    EXPECT_FLOAT_EQ(results_tree.at ("(0, 0, 1)"), initialTree.at ("(0, 0, 1)")); //not affected by disturbances
}