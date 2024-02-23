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

// This file includes unit tests for the class SeedPool. See tests/CMake file 
// and google test documentation.


#include "SeedPool.h"
#include "SeedBiology.h"

/** @cond */
#include "gtest/gtest.h"
/** @endcond */

class SeedPoolTest : public ::testing::Test {
  protected:
  SeedPool* s;
  static const SeedBiology* traits;

  static void SetUpTestSuite() { //creates shared trait Definitions for all tests
    nlohmann::json j = {
        {"Dormancy", false},
        {"GerminationSuccess", 0.1},
        {"MortalityDormant", 0.05},
        {"DormancyBreakRate", 0.8},
    };
    traits = new SeedBiology(j);
    }

  static void TearDownTestSuite() {
    delete traits;
    traits = nullptr;
  }

  void SetUp() override { 
    s = new SeedPool(traits, 100);
  }
  void TearDown() override {
    delete s;
    s = nullptr;
  }
};

const SeedBiology* SeedPoolTest::traits = nullptr;

TEST_F(SeedPoolTest, builds){
    EXPECT_TRUE(s->m_seedBiology_ptr);
    EXPECT_EQ(s->m_capacity, 100);
    EXPECT_FLOAT_EQ(s->m_activeSeeds, 0.0);
    EXPECT_FLOAT_EQ(s->m_dormantSeeds, 0.0);
}

TEST_F(SeedPoolTest, increaseDormant){
    s->increase(10, false);
    EXPECT_EQ(s->m_dormantSeeds, 10);
    s->increase(0,false);
    EXPECT_EQ(s->m_dormantSeeds, 10);
    s->increase(95,false);
    EXPECT_EQ(s->m_dormantSeeds, 100);
}

TEST_F(SeedPoolTest, increaseActive){
    s->increase(10, true);
    EXPECT_EQ(s->m_activeSeeds, 10);
    s->increase(0,true);
    EXPECT_EQ(s->m_activeSeeds, 10);
    s->increase(95,true);
    EXPECT_EQ(s->m_activeSeeds, 100);
}

TEST_F(SeedPoolTest, ageNoSeeds){
    int seeds = s->age(0);
    EXPECT_EQ(seeds, 0);
}

TEST_F(SeedPoolTest,ageNoDisturbance){
    s->increase(100, true);
    int seeds = s->age(0); //10% germinate
    EXPECT_EQ(seeds, 10);
    s->increase(100, false);
    seeds = s->age(0); //80% break dormancy, of the 80 10% germinate
    EXPECT_EQ(seeds, 8); 
    seeds = s->age(0); //1 of the 20 dormants dies (5% mortality)
    EXPECT_EQ(seeds, 2); //19*0.8*0.1 = 1.52, rounded to 2
}

TEST_F(SeedPoolTest,ageInfDisturbance){
    s->increase(10000);
    int seeds = s->age(999999999);
    EXPECT_EQ(seeds, 0);
    seeds = s->age(0);
    EXPECT_EQ(seeds,0); //dormant seed pool was affected as well
}

TEST_F(SeedPoolTest, ageDisturbance){
    s->increase(100, false);
    int seeds = s->age(20); //80% break dormancy, but 20*0.8 seeds are eaten. Of the 64 10% germinate
    EXPECT_EQ(seeds,6);
    seeds = s->age(0); //20*0.2 were eaten, 5% die, 80% break dormancy, 10% germinate
    EXPECT_EQ(seeds,1);//1.2 left
}
//==============================================================================
//below are private functions. Left here for reference and included in the tests, 
//but may outdate eventually. The age() function should suffiently test them. 
TEST_F(SeedPoolTest, decrease){
    s->m_activeSeeds = 10;
    s->m_dormantSeeds = 30;
    int res = s->decrease(8);
    EXPECT_EQ(res, 8);
    EXPECT_EQ(s->m_activeSeeds, 8);
    EXPECT_EQ(s->m_dormantSeeds, 24);

    res = s->decrease(0);
    EXPECT_EQ(res, 0);
    EXPECT_EQ(s->m_activeSeeds, 8);
    EXPECT_EQ(s->m_dormantSeeds, 24);

    s->m_activeSeeds = 10;
    s->m_dormantSeeds = 20;
    res =  s->decrease(5); //not int-divisible
    EXPECT_EQ(res, 5);
    EXPECT_EQ(s->m_activeSeeds, 8);
    EXPECT_EQ(s->m_dormantSeeds, 17);


    res = s->decrease(100000);
    EXPECT_EQ(res, 25);
    EXPECT_EQ(s->m_activeSeeds, 0);
    EXPECT_EQ(s->m_dormantSeeds, 0);
}

TEST_F(SeedPoolTest, decreaseOnlyActive){
    s->m_activeSeeds = 10;
    s->m_dormantSeeds = 30;
    int res = s->decrease(8, true);
    EXPECT_EQ(res, 8);
    EXPECT_EQ(s->m_activeSeeds, 2);
    EXPECT_EQ(s->m_dormantSeeds, 30);

    res = s->decrease(0, true);
    EXPECT_EQ(res, 0);
    EXPECT_EQ(s->m_activeSeeds, 2);
    EXPECT_EQ(s->m_dormantSeeds, 30);

    s->m_activeSeeds = 10;
    s->m_dormantSeeds = 20;
    res =  s->decrease(5, true); //not int-divisible
    EXPECT_EQ(res, 5);
    EXPECT_EQ(s->m_activeSeeds, 5);
    EXPECT_EQ(s->m_dormantSeeds, 20);

    res = s->decrease(100000,true);
    EXPECT_EQ(res, 5);
    EXPECT_EQ(s->m_activeSeeds, 0);
    EXPECT_EQ(s->m_dormantSeeds, 20);
}


TEST_F(SeedPoolTest, germinate){
    int res = s->germinate();
    EXPECT_EQ(res, 0);
    EXPECT_EQ(s->m_dormantSeeds, 0);
    EXPECT_EQ(s->m_activeSeeds, 0);
    s->m_activeSeeds = 10;
    res = s->germinate();
    EXPECT_EQ(res, 1);
    EXPECT_EQ(s->m_activeSeeds, 0);
    s->m_activeSeeds = 16;
    res = s->germinate();
    EXPECT_EQ(res, 2);
    EXPECT_EQ(s->m_activeSeeds, 0);
}