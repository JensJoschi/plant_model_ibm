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

#include "p_FuncGroup.h"
#include "gtest/gtest.h"
#include "p_PropPool.h"
#include "p_PFG.h"
#include "nlohmann/json.hpp"
#include "rng.h"
#include "p_PFGdisturbance.h"

// This file includes unit tests for the class FuncGroup. See tests/CMake file 
// and google test documentation.

class FuncGroupTest : public ::testing::Test {
  protected:
  FuncGroup f;

  static void SetUpTestSuite() { //creates shared pfg Definitions for all tests
    std::cerr << "SETUP CALLED";
    nlohmann::json j = {
      {"Name", "test"}, 
      {"Maturation_time", 2},
      {"Life_Span", 5},
      {"Max_Abund", "Medium"},
      {"ImmSize", 1.0},
      {"MaxStratum", 1},
      {"StrataChangeAge", {0, 2, 10000}},
      {"PoolL", {0,5}},
      {"PotentialFecundity", 2},
      {"LightShadeFactor", 1},
      {"LightActiveGerm", {5, 8, 9}},
      {"LightTolerance", {{true, true, true},{false,true,true},{false,false,true},{false,false,true}}},
      {"SoilTol", {"testsoil"}},
      {"DepthReq", 10}
    };
    pfg = new PFG(j);

    nlohmann::json j2 = {
      {"Name", "test"},
      {"Immature", {
        {"fox", {
          {"SeedKill", 0.1},
          {"PropResprout", 0.0},
          {"PropKilled", 0.0}
        }},
        {"bee", {
          {"SeedKill", 0.0},
          {"PropResprout", 0.0},
          {"PropKilled", 0.9}
        }}
      }},
      {"Mature", {
        {"fox", {
          {"SeedKill", 0.1},
          {"PropResprout", 0.0},
          {"PropKilled", 0.0}
        }},
        {"bee", {
          {"SeedKill", 0.0},
          {"PropResprout", 0.0},
          {"PropKilled", 0.2}
        }}
      }}
    };
    pfgd = new PFGDisturbances(j2);
  }

  static void TearDownTestSuite() {
    delete pfg;
    pfg = nullptr;
  }
  
  void SetUp() override {
    RNGs::mersenne = std::mt19937{2230}; //every test_f calls setup function, so they get the same seed
    const std::vector<int> csize{500, 0, 1, 1}; //up to 500 "individuals" in each age class, age classes are from 0 to 1. 
    f =  FuncGroup (pfg, pfgd, csize, 100, 10, 20, 30); //#seeds in pool, max abundance that low/med/high can reach
  }
  static PFG* pfg;
  static PFGDisturbances* pfgd;
};

PFG* FuncGroupTest::pfg = nullptr;
PFGDisturbances* FuncGroupTest::pfgd = nullptr;

TEST_F(FuncGroupTest, InitialSizeIsCorrect) {
  bool condition = (f.getCount() == 724) || (f.getCount() == 486);
  EXPECT_TRUE(condition)<< "Initial size is not as expected ("<< f.getCount() << "). Mersenne Twister should work on any platform, but FuncGroup calls std::uniform_int_distribution, whose implementation is platform-specific.";
}

TEST_F(FuncGroupTest, getCountWorks) {
  EXPECT_EQ(f.getCount(0,10), f.getCount());
  int size = f.getCount();
  EXPECT_EQ(f.getCount(0,0), size/2); //using size/2 because initial size is platform-dependent (see test InitialSizeIsCorrect)
  EXPECT_EQ(f.getCount(1,1), size/2);
}

TEST_F(FuncGroupTest, stratAbundWorks){
  int size = f.getCount();
  EXPECT_EQ(f.getStratAbund(0), f.getCount()); //all individuals are in stratum 0 (see pfg)
  EXPECT_EQ(f.getStratAbund(1), 0);
  f.age(); //at age 2 they move to stratum 1 (see pfg)
  EXPECT_EQ(f.getStratAbund(0), size/2);
  EXPECT_EQ(f.getStratAbund(1), size/2);
}

TEST_F(FuncGroupTest, ageWorks){
  int size = f.getCount();
  EXPECT_EQ(f.getCount(0,0), size/2);
  EXPECT_EQ(f.getCount(1,1), size/2);
  EXPECT_EQ(f.getCount(2,2), 0);
  f.age(); 
  EXPECT_EQ(f.getCount(1,1), size/2);
  EXPECT_EQ(f.getCount(2,2), size/2);
  f.age();  //oldest have died.
  EXPECT_EQ(f.getCount(1,1), 0);
  EXPECT_EQ(f.getCount(2,2), size/2);
  f.age();
  EXPECT_EQ(f.getCount(0,1), 0);
  EXPECT_EQ(f.getCount(2,2), 0);
}


//   //name
//   assert(f.getName() == "test");
//   //getFecund
//   assert(f.getFecund() == 40);  //individuals are mature from age 2-5, and have fecundity 2 (see pfg). f.getCount(2,5) = 362, but Abundlimit = 20. 20 * fecundity = 40
//   //to do: check that immatures lack fecundity
//   //checkSoilTol
//   assert(f.checkSoilClass("testsoil"));
//   assert(!f.checkSoilClass("othersoil"));
//   assert(f.checkSoilDepth(100));
//   assert(!f.checkSoilDepth(2));

//  // germinate and recruit
//   GSP_PLANTS gsp;
//   gsp.defaultBuild();
//   Resource R(RMedium);
//   f.germinateAndRecruit(&gsp, 10, R,false);
// // 10 new ones coming "in" + 100 old ones  = 110; 
// // current abundance is << 2000, so there is enough space; and we have a 100% germination rate (lightActiveGerm >0.9).
// // ==>110 seeds germinate
// // light tolerance matches light conditions, so do recruitment:
//     //this is a medium-abund pfg, according to gsp a medium abund can have 2000 individuals; so there is space
//     // affected by env is false, so germination is not aborted
//     // ==> create 2110 new individuals of age 0
// // zero remaining seeds age
//   assert(f.getCount() == 110+ f.getCount(1,10));
//   f.age(); //kill oldest cohort, only the 110 1-year olds remain
//   f.germinateAndRecruit(&gsp, 10, R,true); //this time the 10 new seeds (seed pool is empty) germinate but die.
//   assert(f.getCount() == 0+ f.getCount(1,10));
//   f.germinateAndRecruit(&gsp, 10, R,false); //this time the 10 new seeds survive.
//   assert(f.getCount() == 10+ f.getCount(1,10));
//   gsp.maxAbundMedium = 5;
//   f.germinateAndRecruit(&gsp, 10, R,true);    //now can have max 5 germinants; these die. The remaining 5 seeds age (4 remaining)
//   assert(f.getCount() == 10+ 0 +f.getCount(1,10));
//   f.germinateAndRecruit(&gsp, 10, R,false);  //5 germinants, but there is not enough space in the cell (blocked by the 1-year olds). 
//   assert(f.getCount() == 10+ 0 +f.getCount(1,10));
//   f.age();
//   f.age();
//   assert(f.getStratAbund(0) == 0);
//   f.germinateAndRecruit(&gsp, 10, R,false);   // this time there is enough space for 5 new seeds
//   assert(f.getStratAbund(0) == 5);

//   //disturbance
//   LOG (DEBUG) << f.getCount(0,10);
//   std::map<std::string, double> dist{ {"bee", 0.5}, {"fox", 0.5} };
//   f.beDisturbed(dist); //5 immatures, 120 matures. 1 unit of bee kills 90% of the immatures, but there is only 0.5 bee, so 45% of the 5 immatures die. Bee kills also 20% matures * 0.5 bee.
//   assert(f.getCount(0,0) == 2); //not a rounding error, calls ceil()
//   assert(f.getCount(2,2) == 9);
//   assert(f.getCount(3,3) == 99);
//   f.age();
  
//   //die
//   const std::vector<int> csize2{100, 0, 5, 1}; //5 individuals aged 0, 5 individuals aged 1
//   FuncGroup fg{&pfg, &pfgd, csize2, 100, 10, 20, 30};
//   assert(fg.getCount() == 246);
//   fg.die({Resource(RHigh), Resource(RHigh), Resource(RHigh)}); 
//   assert(fg.getCount() == 246); //unaffected by light, noone dies
//   fg.die({Resource(RHigh), Resource(RHigh), Resource(RLow)}); 
//   assert(fg.getCount() == 246); //doesnt live in highest stratum, noone dies. Note: highest stratum in reality always has highest resource, this here is for testing only
//   fg.die({Resource(RHigh), Resource(RMedium), Resource(RLow)}); //0-1 olds are in lowest stratum and survive, older ones live in stratum 2 and die (survive neithr under low nor under medium light)
//   assert(fg.getCount() == 82); 
//   LOG(INFO) << "tests complete.";