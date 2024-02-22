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

// This file includes unit tests for the class Cell. See tests/CMake file 
// and google test documentation.

#include "Cell.h"
#include "PFGDefs.h"
#include "GSP_PLANTS.h"
#include "rng.h"

/** @cond */
#include "gtest/gtest.h"
#include "nlohmann/json.hpp"
/** @endcond */

class CellTest : public ::testing::Test{
  protected: 
  Cell* c;

  static void SetUpTestSuite() { //creates shared pfg Definitions and global parameters for all tests
    nlohmann::json j = {
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
    PFGDisturbances d{j};

    nlohmann::json j2 = {
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
      {"LightTolerance", {{false, false, true},{false,false,true},{false,false,true},{false,false,true}}},
      {"SoilTol", {"testsoil"}},
      {"DepthReq", 10}
    };
    PFG p{j2};
    const std::map<std::string, PFG> pfgMap{{"test", p}};
    const std::map<std::string, PFGDisturbances> pfgDistMap{{"test", d}};
    pfg = new PFGDefs(pfgMap, pfgDistMap);

    gsp = new GSP_PLANTS();
    gsp->defaultBuild();
    gsp->noStrata = 3;
    gsp->strataHeight = {33, 33, 33};
  }

  void SetUp() override {
    RNGs::mersenne = std::mt19937{2230}; //every test_f calls setup function, so they get the same seed
    const std::vector<int> csize{500, 0, 1, 1}; //up to 500 "individuals" in each age class, age classes are from 0 to 1. 
    c = new Cell(gsp, pfg, 0.5, 10, csize, 100); //vars = gsp, pfg, shading, soildepth, funcgroup size, initial seedpool 
  }

  static void TearDownTestSuite() {
    delete pfg;
    pfg = nullptr;
    delete gsp;
    gsp = nullptr;
  }
  
  static PFGDefs* pfg;
  static GSP_PLANTS* gsp;
};

PFGDefs* CellTest::pfg = nullptr;
GSP_PLANTS* CellTest::gsp = nullptr;

TEST_F(CellTest, getPFGabund){
  bool condition = (c->getPFGabund(0) == 724 || c->getPFGabund(0) == 486);
  EXPECT_TRUE(condition) << "Initial size is not as expected ("<< c->getPFGabund(0) << "). Mersenne Twister should work on any platform, but FuncGroup calls std::uniform_int_distribution, whose implementation is platform-specific.";
}

TEST_F(CellTest, DoSuccession){
  int initial  = c->getPFGabund(0);
  c->CreateHabSuit(false, "testsoil"); //correct soil class
  std::map<std::string, int> seeds_to_distribute = c->DoSuccession({{"test", 100}}); //100 seeds come from region
  EXPECT_EQ(seeds_to_distribute.at("test"), initial); //one of the two age cohorts becomes mature; fecundity ==2, hence 1/2*initial * 2
  EXPECT_EQ(c->getPFGabund(0) , initial+200); //100 from regional model, 100 from seed pool (created at initialization)

  seeds_to_distribute = c->DoSuccession({{"test", 100}});
  EXPECT_EQ(seeds_to_distribute.at("test"), initial* 2); //both age cohorts mature, fecundity ==2
  EXPECT_EQ(c->getPFGabund(0), initial+200+100); //only the 100 from regional model now (initial seed pool is spent)
}

TEST_F(CellTest, SeedPoolAging){
  int initial  = c->getPFGabund(0);
  c->CreateHabSuit(false, "testsoil");
  std::map<std::string, int> seeds_to_distribute = c->DoSuccession({{"test", 2000}}); //insert exactly MaxAbund seeds, so seed pool is not touched; but ages by one time step
  EXPECT_EQ(c->getPFGabund(0) , initial+2000); //does not include seed pool
  seeds_to_distribute = c->DoSuccession({{"test", 0}}); //causes seed pool to be spent
  EXPECT_EQ(c->getPFGabund(0) , initial +2000 + 83); //seed pool was 100, aged by one time step with pfg::seedPoolLifeSpan == 5; see PropPoolTest::AgePoolL5
}

TEST_F(CellTest, TooHighSeedInput){
  int initial  = c->getPFGabund(0);
  c->CreateHabSuit(false, "testsoil");
  std::map<std::string, int> seeds_to_distribute = c->DoSuccession({{"test", 100000000}}); 
  EXPECT_EQ(c->getPFGabund(0), initial + 2000); //PFG is medium size, GSP::maxAbund == 2000
  seeds_to_distribute = c->DoSuccession({{"test", 0}}); //no seeds from region...
  EXPECT_EQ(c->getPFGabund(0), initial + 2000 + 2000); //...but seed pool was filled during last call, they are used now
}

TEST_F(CellTest, Aging){
  int initial  = c->getPFGabund(0);
  c->CreateHabSuit(false, "testsoil");
  std::map<std::string, int> seeds_to_distribute = c->DoSuccession({{"test", 0}});
  EXPECT_EQ(c->getPFGabund(0), initial+100); //100 from seed pool (created at initialization)
  for (int i =0; i<3; i++){
    seeds_to_distribute = c->DoSuccession({{"test", 0}});
  }
  EXPECT_EQ(c->getPFGabund(0), initial+100);
  seeds_to_distribute = c->DoSuccession({{"test", 0}});
  EXPECT_EQ(c->getPFGabund(0), initial/2+100); //oldest cohort died
  seeds_to_distribute = c->DoSuccession({{"test", 0}});
  EXPECT_EQ(c->getPFGabund(0), 100); //next cohort died
  seeds_to_distribute = c->DoSuccession({{"test", 0}});
  EXPECT_EQ(c->getPFGabund(0), 0); //seed pool cohort died
}

TEST_F(CellTest, Neighbour1){
  int initial  = c->getPFGabund(0);
  gsp->doesNeighbourinfluence = true;
  gsp->lightAngle = 40;
  const std::vector<int> noNeighbour{0,0,0};
  c->CreateHabSuit(false, "testsoil");
  std::map<std::string, int> seeds_to_distribute = c->DoSuccession({{"test", 1900}}, noNeighbour); 
  EXPECT_EQ(seeds_to_distribute.at("test"), initial);
  EXPECT_EQ(c->getPFGabund(0) , initial+2000); 
  seeds_to_distribute = c->DoSuccession({{"test", 100}});
  EXPECT_EQ(seeds_to_distribute.at("test"), initial* 2); 
  EXPECT_EQ(c->getPFGabund(0), initial+2000+100); 

  gsp->doesNeighbourinfluence = false;
  gsp->lightAngle = 90;
}

TEST_F(CellTest, Neighbour2){
  int initial  = c->getPFGabund(0);
  gsp->doesNeighbourinfluence = true;
  gsp->lightAngle = 40;
  const std::vector<int> smallNeighbour{0,5000,0}; //not realistic, testing only
  c->CreateHabSuit(false, "testsoil");
  std::map<std::string, int> seeds_to_distribute = c->DoSuccession({{"test", 1900}}, smallNeighbour); 
  EXPECT_EQ(seeds_to_distribute.at("test"), initial);
  EXPECT_EQ(c->getPFGabund(0) , initial); //the 2000 new germinants die. Compare with Neighbour1
  seeds_to_distribute = c->DoSuccession({{"test", 100}});
  EXPECT_EQ(seeds_to_distribute.at("test"), initial* 2); 
  EXPECT_EQ(c->getPFGabund(0), initial+100); 

  gsp->doesNeighbourinfluence = false;
  gsp->lightAngle = 90;
}

TEST_F(CellTest, Neighbour3){
  int initial  = c->getPFGabund(0);
  gsp->doesNeighbourinfluence = true;
  gsp->lightAngle = 40;
  const std::vector<int> largeNeighbour{0,0,10000};
  c->CreateHabSuit(false, "testsoil");
  std::map<std::string, int> seeds_to_distribute = c->DoSuccession({{"test", 1900}}, largeNeighbour); 
  EXPECT_EQ(seeds_to_distribute.at("test"), initial);
  EXPECT_EQ(c->getPFGabund(0) , initial); //germinants die. Compare with Neighbour1
  seeds_to_distribute = c->DoSuccession({{"test", 100}});
  EXPECT_EQ(seeds_to_distribute.at("test"), initial/2* 2); //the oldest have moved up to highest stratum and thus survive, younger cohort and new germinants die
  EXPECT_EQ(c->getPFGabund(0), initial/2+100); 

  gsp->doesNeighbourinfluence = false;
  gsp->lightAngle = 90;
}



TEST_F(CellTest, createHabSuit1){
  int initial  = c->getPFGabund(0);
  c->CreateHabSuit(true, "testsoil");
  //although seeds die on the wrong soil, we here use the correct soil and expect no difference to DoSuccession test
  std::map<std::string, int> seeds_to_distribute = c->DoSuccession({{"test", 100}}); 
  EXPECT_EQ(seeds_to_distribute.at("test"), initial); 
  EXPECT_EQ(c->getPFGabund(0) , initial+200); 

  seeds_to_distribute = c->DoSuccession({{"test", 100}});
  EXPECT_EQ(seeds_to_distribute.at("test"), initial* 2);
  EXPECT_EQ(c->getPFGabund(0), initial+200+100); 
}

TEST_F(CellTest, createHabSuit2){
  int initial  = c->getPFGabund(0);
  c->CreateHabSuit(false, "wrongSoil"); //while plants will not die, no new seeds will germinate
  std::map<std::string, int> seeds_to_distribute = c->DoSuccession({{"test", 100}}); 
  EXPECT_EQ(seeds_to_distribute.at("test"), initial); 
  EXPECT_EQ(c->getPFGabund(0) , initial); //no new seeds

  seeds_to_distribute = c->DoSuccession({{"test", 100}});
  EXPECT_EQ(seeds_to_distribute.at("test"), initial* 2); //fecundity not affected
  EXPECT_EQ(c->getPFGabund(0), initial); //no new seeds
}

TEST_F(CellTest, createHabSuit3){
  c->CreateHabSuit(true, "wrongSoil"); //plants die and no new seeds will germinate
  std::map<std::string, int> seeds_to_distribute = c->DoSuccession({{"test", 100}}); 
  EXPECT_EQ(seeds_to_distribute.at("test"), 0); 
  EXPECT_EQ(c->getPFGabund(0) , 0);
}

TEST_F(CellTest, DisturbanceWorks){
 int initial  = c->getPFGabund(0);
  std::map<std::string, double> dist{ {"bee", 0.5}, {"fox", 0.5} };
  c->CreateDisturbances(dist);
  c->CreateHabSuit(false, "testsoil");
  std::map<std::string, int> seeds_to_distribute = c->DoSuccession({{"test", 100}});
  int reduced = initial*0.55; //see FuncGroupTest::disturbanceWorks for rounding and calculation
  EXPECT_GE(c->getPFGabund(0), reduced +200 -2); //200 new germinants
  EXPECT_LE(c->getPFGabund(0), reduced +200 +2);

  seeds_to_distribute = c->DoSuccession({{"test", 100}});
  EXPECT_GE(c->getPFGabund(0), 0.725 * reduced + 210 -2); //fractions of mature, immature and germinants were calculated by hand
  EXPECT_LE(c->getPFGabund(0), 0.725 * reduced + 210 +2);
}