/*------------------------------------------------------------------------------
Copyright (C)  2022 - present  Studio Animal-Aided Design

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

#include "FuncGroup.h"
#include "gtest/gtest.h"
#include "PropPool.h"
#include "PFG.h"
#include "nlohmann/json.hpp"
#include "rng.h"
#include "PFGdisturbance.h"

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
    gsp = new GSP_PLANTS();
    gsp->defaultBuild();
  }

  static void TearDownTestSuite() {
    delete pfg;
    pfg = nullptr;
    delete pfgd;
    pfgd = nullptr;
    delete gsp;
    gsp = nullptr;
  }
  
  void SetUp() override {
    RNGs::mersenne = std::mt19937{2230}; //every test_f calls setup function, so they get the same seed
    const std::vector<int> csize{500, 0, 1, 1}; //up to 500 "individuals" in each age class, age classes are from 0 to 1. 
    f =  FuncGroup (pfg, pfgd, csize, 100, 10, 20, 30); //#seeds in pool, max abundance that low/med/high can reach
  }
  static PFG* pfg;
  static PFGDisturbances* pfgd;
  static GSP_PLANTS* gsp;
};

PFG* FuncGroupTest::pfg = nullptr;
PFGDisturbances* FuncGroupTest::pfgd = nullptr;
GSP_PLANTS* FuncGroupTest::gsp = nullptr;

TEST_F(FuncGroupTest, InitialSizeIsCorrect) {
  bool condition = (f.getCount() == 724) || (f.getCount() == 486);
  EXPECT_TRUE(condition)<< "Initial size is not as expected ("<< f.getCount() << "). Mersenne Twister should work on any platform, but FuncGroup calls std::uniform_int_distribution, whose implementation is platform-specific.";
}


// *************** GETTERS *****************

TEST_F(FuncGroupTest, getCountWorks) {
  EXPECT_EQ(f.getCount(0,10), f.getCount());
  int size = f.getCount();
  EXPECT_GE(f.getCount(0,0), size/2 - 1);//using size/2 because initial size is platform-dependent (see test InitialSizeIsCorrect)
  EXPECT_LE(f.getCount(0,0), size/2 + 1); //see issue 33 for rounding errors
  EXPECT_GE(f.getCount(1,1), size/2 - 1);//using size/2 because initial size is platform-dependent (see test InitialSizeIsCorrect)
  EXPECT_LE(f.getCount(1,1), size/2 + 1); //see issue 33 for rounding errors
}

TEST_F(FuncGroupTest, stratAbundWorks){
  int size = f.getCount();
  EXPECT_EQ(f.getStratAbund(0), f.getCount()); //all individuals are in stratum 0 (see pfg)
  EXPECT_EQ(f.getStratAbund(1), 0);
  f.age(); //at age 2 they move to stratum 1 (see pfg)
  EXPECT_GE(f.getStratAbund(0), size/2 - 1);
  EXPECT_LE(f.getStratAbund(0), size/2 + 1);
  EXPECT_EQ(f.getStratAbund(1), size/2);
  EXPECT_GE(f.getStratAbund(1), size/2 - 1);
  EXPECT_LE(f.getStratAbund(1), size/2 + 1);
}

TEST_F(FuncGroupTest, getNameWorks){
  EXPECT_EQ(f.getName(), "test");
}

TEST_F(FuncGroupTest, getFecundWorks){
  EXPECT_EQ(f.getFecund(), 0);
  int size = f.getCount();
  f.age();
  f.age();
  EXPECT_EQ(f.getCount(2,2), size/2); //half of the individuals are mature
  //FuncGroup was set up with AbundLimit of 10/20/30 depending on the PFGs Abund;

  EXPECT_EQ(f.getFecund(), 40); //pfg::Abund = medium (->20); pfg::fecundity = 2 (->40)
}

TEST_F(FuncGroupTest, checkSoilClassWorks){ //pfg::SoilTol = "testsoil"
  EXPECT_TRUE(f.checkSoilClass("testsoil"));
  EXPECT_FALSE(f.checkSoilClass("othersoil"));
}

TEST_F(FuncGroupTest, checkSoilDepthWorks){ //pfg::Depthreq = 10
  EXPECT_TRUE(f.checkSoilDepth(100));
  EXPECT_FALSE(f.checkSoilDepth(2));
}


//*************** FUNCTIONS *****************
TEST_F(FuncGroupTest, ageWorks){
  int size = f.getCount();
  EXPECT_GE(f.getCount(0,0), size/2 - 1);
  EXPECT_LE(f.getCount(0,0), size/2 + 1);
  EXPECT_GE(f.getCount(1,1), size/2 - 1);
  EXPECT_LE(f.getCount(1,1), size/2 + 1);
  EXPECT_EQ(f.getCount(2,2), 0);
  f.age(); 
  EXPECT_GE(f.getCount(1,1), size/2 - 1);
  EXPECT_LE(f.getCount(1,1), size/2 + 1);
  EXPECT_GE(f.getCount(2,2), size/2 - 1);
  EXPECT_LE(f.getCount(2,2), size/2 + 1);
  f.age();  //oldest have died.
  EXPECT_EQ(f.getCount(1,1), 0);
  EXPECT_GE(f.getCount(2,2), size/2 - 1);
  EXPECT_LE(f.getCount(2,2), size/2 + 1);
  f.age();
  EXPECT_EQ(f.getCount(0,1), 0);
  EXPECT_EQ(f.getCount(2,2), 0);
}

TEST_F(FuncGroupTest, germinateWorks){
  int size = f.getCount();
  Resource R(RMedium);
  f.germinateAndRecruit(gsp, 10, R,false); //funcGroup was setup with 100 seeds in pool, 10 new ones come in = 110
  EXPECT_GE(f.getCount(), size + 110 - 1);
  EXPECT_LE(f.getCount(), size + 110 + 1);
  //same as above but there are more seeds than gsp::MaxAbund, so only gsp::Maxabund germinate
  f.germinateAndRecruit(gsp, 100000, R,false);
  EXPECT_GE(f.getCount(), size + 110 + 2000 - 1); //includes the 110 from above
  EXPECT_LE(f.getCount(), size + 110 + 2000 + 1);
  f.age();
}

TEST_F(FuncGroupTest, envKillsGerminants){
  int size = f.getCount();
  Resource R(RMedium);
  f.germinateAndRecruit(gsp, 10, R,true); //they do not survive, because affected by env
  EXPECT_EQ(f.getCount(), size);  //compare with germinateWorks
  f.germinateAndRecruit(gsp, 100000, R,true); 
  EXPECT_EQ(f.getCount(), size);
}

TEST_F (FuncGroupTest, disturbanceWorks){
  //"bee" should kill 90% of the immatures and 20% of the matures (see pfgd), fox 0%
  int size = f.getCount();
  std::map<std::string, double> dist{ {"bee", 0.5}, {"fox", 0.5} };
  f.beDisturbed(dist); 
  int reduced = size - ceil(size * 0.5 * 0.9);
  EXPECT_GE(f.getCount(), reduced - 2); //see issue 33 for rounding errors  
  EXPECT_LE(f.getCount(), reduced + 2); //added additional leeway here because of multiple INT*DOUBLE operations
  f.age(); //half of the individuals are mature now
  f.beDisturbed(dist); 
  int immaturePart = 0.5 * reduced - ceil(0.5 * reduced * 0.5 * 0.9); 
  int maturePart =   0.5 * reduced - ceil(0.5 * reduced * 0.5 * 0.2);
  EXPECT_GE(f.getCount(),  maturePart + immaturePart - 2);
  EXPECT_LE(f.getCount(),  maturePart + immaturePart + 2);
}

TEST_F(FuncGroupTest, dieWorks){
  int size = f.getCount();
  f.die({Resource(RHigh), Resource(RHigh), Resource(RHigh)});
  EXPECT_EQ(f.getCount(), size); //unaffected by light, noone dies
  f.die({Resource(RHigh), Resource(RHigh), Resource(RLow)});
  EXPECT_EQ(f.getCount(), size); //doesnt live in highest stratum, noone dies.
  //Note: highest stratum in reality always has highest resource, this here is for testing only
  f.age(); //oldest individuals are in stratum 2
  f.die({Resource(RHigh), Resource(RMedium), Resource(RHigh)}); //0-1 olds are in lowest stratum and survive, older ones live in stratum 2 and die (survive neithr under low nor under medium light)
  EXPECT_GE(f.getCount(), size/2 - 1);
  EXPECT_LE(f.getCount(), size/2 + 1);
}
