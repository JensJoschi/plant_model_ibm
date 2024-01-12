/*------------------------------------------------------------------------------
 * 
 * Copyright (C) 2022 Jens Joschinski
 * 
 * This file is part of the Ecolopes Ecological model. The model represents the 
 * joint effort of several ECOLOPES partners (Wolfgang Weisser, Anne Mimet, Jens
 * Joschinski, Victoria Culshaw), incorporating existing code and conceptual help 
 * from Carsten Buchmann and the RFate developers (particularly Isabelle Boulangeat
 * and Maya Gueguen).
 * For enquiries regarding copyright, please contact the Ecolopes team: 
 * info@ecolopes.org
 * Individual file owners and authorships are mentioned in the header files.
 */
 
 // ----------------------------------------------------------------------------
 // Authors and contributors to this file:
 // JJ: logic and code of main(); documentation and commenting of shared parts, 
 //     plant and soil model; conceptual connection with architecture workflow (env. models)
 // VMVC: logic and code of main(); documentation of animal model
 // Anne Mimet: conceptual connection with architecture workflow, general oversight
// ----------------------------------------------------------------------------

/*! \mainpage Ecolopes Model
 *
 * \section intro_sec Introduction
 *
 * This repository hosts the ECOLOPES plant-animal-soil model, tasks 4.2.-4.4 (partly including 4.5). It is meant for use 
 * within the computational workflow (WP3) of ECOLOPES, but we may in the future publish it as stand-alone model for research 
 * use as well.
 * For further information about the model, click on README under the table of contents on the left.
 *
 * \section copyright_sec Copyright
 * Copyright (C) Victoria Culshaw,Jens Joschinski, Anne Mimet and Wolfgang Weisser - All Rights Reserved
 * Unauthorized copying of the Ecolopes Ecological Model via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "p_PropPool.h"
#include <string>
#include <random>
#include <filesystem>
#include <cassert>
#include "p_PFG.h"
#include "p_PFGdisturbance.h"
#include "p_Cohort.h"
#include "p_FuncGroup.h"
#include "p_PFG.h"
#include "p_Cell.h"
#include "p_PFGDefs.h"
#include "p_PlantModel.h"
#include "easylogging++.h"
#include "Landscape.h"
#include "rng.h"
#include "p_ResourceAlloc.h"
#include "p_PlantResource.h"  
#include "p_Individual.h"
//get default WD from compiler flag
#ifndef DEFAULT_WD
std::string default_WD = "/Users/jensjoschinski/rfate";
#else
std::string default_WD = DEFAULT_WD;
#endif

INITIALIZE_EASYLOGGINGPP

/*============================================================================
Main-------------------------------------------------------------------------
---------------------------------------------------------------------------*/
int main(int argc, char *argv[]){
    el::Configurations conf(default_WD + "/log.conf"); 
  el::Configurations plants_conf(default_WD + "/log_plants.conf"); 
  el::Loggers::getLogger("PLANTS");
  el::Loggers::reconfigureAllLoggers(conf); 
  el::Loggers::reconfigureLogger("PLANTS", plants_conf);
  el::Loggers::addFlag(el::LoggingFlag::MultiLoggerSupport);

    std::filesystem::current_path(default_WD);
  RNGs::mersenne = std::mt19937{2230};

  LOG(INFO) << "RUNNING TESTS";
    LOG(INFO) << "--Landscape class.--";
  //the constructors which read from file support 4 types of information:
  //normal key-value pairs;
  //key value pairs where the value is in an array of size 1;
  //key-value pairs in which the value is a json + information which element to fetch from the json;
  //key-value pairs with jsons, but wrapped into an array of size 1

  //create landscape <int> using information {(0,0,0): 1}
  Landscape<int> l{default_WD + "/landscape1.json" };
  assert(l.at("(1, 2, 1)") == 16);
  assert(l.at("(1, 1, 3)") == 4);

  //create landscape <int> using information {(0,0,0): [1]}
  Landscape<int> l2{default_WD + "/landscape2.json"};
  assert(l2.at("(1, 2, 1)") == 16);

  // //create landscape <int> using information {(0,0,0): {SHADING: 1, DEPTH: 2}}
  Landscape<int> l3{default_WD + "/landscape3.json", "DEPTH" };
  assert(l3.at("(1, 2, 1)") == 185);

  // //create landscape <int> using information {(0,0,0): [{SHADING: 1, DEPTH: 2}] }
  Landscape<int> l4{default_WD + "/landscape4.json", "DEPTH"};
  assert(l4.at("(1, 2, 1)") == 185);

  // this constructor creates a landscape by passing a vector keys (inserts null values)
  std::vector<std::string> keys = {"(0, 0, 1)", "(7, 2, 1)"};
  Landscape<int> l5(keys);
  assert(l5.at("(0, 0, 1)") == 0);

  // this constructor creates a landscape using key -value pairs
  std::map<std::string, bool> m = {{"(0, 0, 1)", false}, {"(7, 2, 1)", true}};
  Landscape<bool> l6(m);
  assert(l6.at("(7, 2, 1)"));

  //other landscape functions
  l5.setValue("(7, 2, 1)", 1);
  assert(l5.at("(7, 2, 1)") == 1);
  std::vector<std::string> k  = l6.getKeys();
  assert(k[0] == "(0, 0, 1)");

  //range based lookup:
  int sum = 0;
  for (auto& [key, value] : l) {
    sum += value;
  }
    assert(sum == 135);

  for (const auto& item: l){
    sum += item.second;
  }
assert(sum = 270);

  LOG(INFO) << "#### 1  Plant model. #### ";
  LOG(INFO) << "---Create a PFG---";
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

  PFG pfg(j);
  LOG(INFO) << "---Create Resources---";
  //Resources and Individuals are part of a new model. They are not yet used in the PlantModel, but will be in the future.
  const ResourceAlloc allocation;  //normally this is part of the PFG definition
  PlantResource pr(&allocation);
  assert(pr.isResourceCritical());
  assert (pr.updateResource(100) == 100);
  assert (!pr.isResourceCritical());
  Allocations a = pr.allocateResources(100); //assuming plant biomass ==100
  assert(a.biomass == 5);
  assert(a.seeds == 0);
  assert (!pr.isResourceCritical());
  a = pr.allocateResources(21);
  assert(a.biomass == 1);
  a = pr.allocateResources(21000000); //biomass so large that maintenance over-depletes resources
  assert(pr.isResourceCritical());
  assert(a.biomass == 0);
  assert(a.seeds == 0);
  PlantResource pr2(&allocation);
  pr2.updateResource(10000000);
  a = pr2.allocateResources(1000);
  assert(a.biomass == 50); //max investment, independent of resources
  assert(a.seeds == (10000000- 1000*0.1)*0.01); //no max investment, only resource-limited

  Individual i(&pfg);
  i.feed(100, true);
  LOG(INFO) << "---Create PFG Disturbances---";
  //create disturbance json (affecting only "test" PFG)
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
  PFGDisturbances pfgd(j2);

  LOG(INFO) << "---Create a PROPPOOL---";
  // this class implements the seed pool. I think it needs a rewrite. See comments in propPool.h
  PropPool p(100,true, 1); //creates a seed mass of 100, which declines over time due to mortality
  assert(p.getSize() == 100);
  p.AgePool1(1); //Aging the pool means calculating seed mortality.
  //the value "1" describes something like the half-life of the seed mass, a value of 1 causes 50% mortality. 
  //A value of 2 causes only 33% mortality, because seeds live on average 2 years (0.66*0.66 = 0.44).
  //a value of 3 causes 25% mortality, because seeds live on average 3 years (0.75*0.75*0.75 = 0.42)
  //the formula is not correct but close enough for now.... 
  assert(p.getSize()==50); //50% have died with AgePool1(1)
  p.PutSeedInPool(37);
  assert(p.getSize()==50); //new seed pool is smaller, so it is not replaced
  p.PutSeedInPool(137);
  assert(p.getSize()==137); //new seed pool is larger, so it is replaced
  //This is odd, i think it would make more sense if the results were 37 and 187 here.
  //but it is consistent so i leave it as is for now.


  LOG(INFO) << "---Create a FUNCGROUP---";
  //the funcGroup contains the demography of one PFG in one cell (biomass of different ages).
  //it also contains the seed pool. There data is stored as "cohort" objects.
  const std::vector<int> csize{500, 0, 1, 1}; //5 individuals aged 0, 5 individuals aged 1. These are not actual individuals, but biomass units. I'll call them "individuals" nevertheless
  FuncGroup f{&pfg, &pfgd, csize, 100, 10, 20, 30}; //#seeds in pool, max abundance that low/med/high can reach
  // getCount
  assert (f.getCount() == 724); //using RNG::mersenne seed 2230
  assert (f.getCount(0,10) == f.getCount());
  assert (f.getCount(1,1)  == 362);
  // getStratAbund
  assert (f.getStratAbund(0) == f.getCount()); //all individuals are in stratum 0 (see pfg)
  assert (f.getStratAbund(1) == 0);
  f.age(); //at age 2 they move to stratum 1 (see pfg)
  assert(f.getCount(1,2) == 724);
  assert(f.getStratAbund(0) == f.getCount(0,1));
  assert(f.getStratAbund(1) == 362);
  // age
  for (int i=0; i<3; i++) f.age();
  assert(f.getCount() == 724);
  f.age();
  assert(f.getCount() == 362); //oldest have died.
  //name
  assert(f.getName() == "test");
  //getFecund
  assert(f.getFecund() == 40);  //individuals are mature from age 2-5, and have fecundity 2 (see pfg). f.getCount(2,5) = 362, but Abundlimit = 20. 20 * fecundity = 40
  //to do: check that immatures lack fecundity
  //checkSoilTol
  assert(f.checkSoilClass("testsoil"));
  assert(!f.checkSoilClass("othersoil"));
  assert(f.checkSoilDepth(100));
  assert(!f.checkSoilDepth(2));

 // germinate and recruit
  GSP_PLANTS gsp;
  gsp.defaultBuild();
  Resource R(RMedium);
  f.germinateAndRecruit(&gsp, 10, R,false);
// 10 new ones coming "in" + 100 old ones  = 110; 
// current abundance is << 2000, so there is enough space; and we have a 100% germination rate (lightActiveGerm >0.9).
// ==>110 seeds germinate
// light tolerance matches light conditions, so do recruitment:
    //this is a medium-abund pfg, according to gsp a medium abund can have 2000 individuals; so there is space
    // affected by env is false, so germination is not aborted
    // ==> create 2110 new individuals of age 0
// zero remaining seeds age
  assert(f.getCount() == 110+ f.getCount(1,10));
  f.age(); //kill oldest cohort, only the 110 1-year olds remain
  f.germinateAndRecruit(&gsp, 10, R,true); //this time the 10 new seeds (seed pool is empty) germinate but die.
  assert(f.getCount() == 0+ f.getCount(1,10));
  f.germinateAndRecruit(&gsp, 10, R,false); //this time the 10 new seeds survive.
  assert(f.getCount() == 10+ f.getCount(1,10));
  gsp.maxAbundMedium = 5;
  f.germinateAndRecruit(&gsp, 10, R,true);    //now can have max 5 germinants; these die. The remaining 5 seeds age (4 remaining)
  assert(f.getCount() == 10+ 0 +f.getCount(1,10));
  f.germinateAndRecruit(&gsp, 10, R,false);  //5 germinants, but there is not enough space in the cell (blocked by the 1-year olds). 
  assert(f.getCount() == 10+ 0 +f.getCount(1,10));
  f.age();
  f.age();
  assert(f.getStratAbund(0) == 0);
  f.germinateAndRecruit(&gsp, 10, R,false);   // this time there is enough space for 5 new seeds
  assert(f.getStratAbund(0) == 5);

  //disturbance
  LOG (DEBUG) << f.getCount(0,10);
  std::map<std::string, double> dist{ {"bee", 0.5}, {"fox", 0.5} };
  f.beDisturbed(dist); //5 immatures, 120 matures. 1 unit of bee kills 90% of the immatures, but there is only 0.5 bee, so 45% of the 5 immatures die. Bee kills also 20% matures * 0.5 bee.
  assert(f.getCount(0,0) == 2); //not a rounding error, calls ceil()
  assert(f.getCount(2,2) == 9);
  assert(f.getCount(3,3) == 99);
  f.age();
  
  //die
  const std::vector<int> csize2{100, 0, 5, 1}; //5 individuals aged 0, 5 individuals aged 1
  FuncGroup fg{&pfg, &pfgd, csize2, 100, 10, 20, 30};
  assert(fg.getCount() == 246);
  fg.die({Resource(RHigh), Resource(RHigh), Resource(RHigh)}); 
  assert(fg.getCount() == 246); //unaffected by light, noone dies
  fg.die({Resource(RHigh), Resource(RHigh), Resource(RLow)}); 
  assert(fg.getCount() == 246); //doesnt live in highest stratum, noone dies. Note: highest stratum in reality always has highest resource, this here is for testing only
  fg.die({Resource(RHigh), Resource(RMedium), Resource(RLow)}); //0-1 olds are in lowest stratum and survive, older ones live in stratum 2 and die (survive neithr under low nor under medium light)
  assert(fg.getCount() == 82); 
  LOG(INFO) << "tests complete.";
  
  LOG(INFO) << "---Create a Succession model---";
  //requires a PFGDefs:
  std::map<std::string, PFG> pfgs;
  pfgs.insert(std::pair<std::string, PFG>("test", pfg));
  std::map<std::string, PFGDisturbances> pfgds;
  pfgds.insert(std::pair<std::string, PFGDisturbances>("test", pfgd));
  PFGDefs pfgdefs(pfgs, pfgds);
  gsp.defaultBuild(); //sets max abundances back to 1000/2000/3000
  gsp.noStrata = 3;
  gsp.strataHeight = {25,25,25};
  gsp.lightThreshLow = 1000; //one pfg can cause low light levels below now
  Cell succ(&gsp, &pfgdefs, 0.5, 10, csize, 100);
  // std::vector<int> neighbour = {0,0,0,0};
  // int myresults = succ.calculateLight(100, 90, 2132132121, 10);
  // myresults = succ.calculateLight(100, 70, 2000, 10);
  // myresults = succ.calculateLight(100, 45, 40000, 10);
  // myresults = succ.calculateLight(100, 20, 0, 10);
  // myresults = succ.calculateLight(100, 1, 2132132121, 10);  //my hand-drawings give same results
  // succ.updateLight(neighbour, 90); //here grows nothing
  // neighbour = {1000000,1000000,1000000,100000};
  // succ.updateLight(neighbour, 90); //still nothing, angle = 90
  // succ.updateLight(neighbour, 20); //low resource everyhwere

  //Cell has two "setter" functions, CreateHabsuit and CreateDisturbance.
  //It further has a function DoSuccession, which does all the calculations (
  //  changing internal state, i.e. abundance of the funcgroups; and outputting 
  //  the number of seeds produced by each PFG). 
  //Lastly, Cell has the getter function getPFGabund.

  //test getter:
  assert(succ.getPFGabund(0) == 240); //with rng seed 2230

//save current RNG state for later restoring. 
//this is neceesary because the RNG was added to Cell::UpdateEnvSuitRefVal() after writing 
//various tests. Now that the drawing of two additional numbers changes the RNG state for 
//for all tests further down (line 488, plant model).
std::mt19937 temp = RNGs::mersenne;

  //test DoSuccession with valid CreateHabSuit setter
  //=====================================================
  succ.CreateHabSuit(false, "testsoil");
  int res = succ.getPFGabund(0); 
  assert(res == 240);
  std::map<std::string, int> seeds_to_distribute = succ.DoSuccession({{"test", 100}});
  res = succ.getPFGabund(0);     // 100 seeds come from neighbouring cells (argument), 
                                 // 100 are in the seed pool(succc constructor).
                                 // the existing cohort grows to 120(2), 120(1), Noone dies of light. 
                                 // new: 120(2), 120(1), 200(0) =440
  assert(res == 440);
  assert(seeds_to_distribute["test"] = 240);  //120(2) * 2 (fecundity) = 240
  // ------------------------------
  seeds_to_distribute = succ.DoSuccession({{"test", 100}}); 
  res = succ.getPFGabund(0);     // 100 seeds come from neighbouring cells (argument), 
                                 // the seed pool is empty (no seeds remaining from previous years)
                                  //existing cohort grows to 120(3), 120(2), 200(1), Noone dies of light
                                  //new: 120(3), 120(2), 200(1), 100(0) = 540
  assert(res == 540);
  assert (seeds_to_distribute["test"] == 480); //120(3) * 2 (fecundity) + 120(2) * 2 => 480

  // ------------------------------
  seeds_to_distribute = succ.DoSuccession({{"test", 2100}}); 
  res = succ.getPFGabund(0);     // 2100 seeds come from neighbouring cells (argument), 
                                 // the seed pool is empty (no seeds remaining from previous years)
                                  //existing cohort grows to 120(4), 120(3), 200(2),100(1). Noone dies of light
                                  //new seeds would be 2100, but because this is a medium-abund plant (pfg attribute), and gsp.maxAbundMedium is 2000, only 2000 germinate.
                                  //100 go back to the seed pool(internal to succession model)
                                  //new: 120(4), 120(3), 200(2),100(1), 2000 (0) = 2540
  assert(res == 2540);
  assert (seeds_to_distribute["test"] == 880); //120(4)*2 + 120(3) * 2 + 200(2) * 2 => 880

  // ------------------------------
  seeds_to_distribute = succ.DoSuccession({{"test", 100}});  //tthe 100 seeds from the seed pool age, 83 remain (PoolL is 5, p.age(5) gives 83)
  res = succ.getPFGabund(0);     //new:120(5), 120(4), 200(3),100(2), 2000(1), 100+83 = 2723
  assert(res == 2723);
  assert (seeds_to_distribute["test"] == 1080); //540*2 = 1080

  // ------------------------------
  seeds_to_distribute = succ.DoSuccession({{"test", 100}}); 
  res = succ.getPFGabund(0);     // oldest cohort dies. Additionally, the 2000's become 2 years old and grow to startum 2. Stratum 0 has now low light.
                                // new: 120(5), 200(4),100(3), 2000(2), 183(1), 0(0) = 2603   --> note no 120(6) and 0(0)
  assert(res == 2603);
  assert (seeds_to_distribute["test"] == 4000); //2620 * 2 = 5240. Max is limited to 2000 * 2(fecundity),however

  // ------------------------------

  seeds_to_distribute = succ.DoSuccession({{"test", 100}}); 
  res = succ.getPFGabund(0);     // oldest cohort dies. the cohort growing below the 2000's also dies. 
                                 // new: 200(5), 100(4), 2000(3) = 2300
  assert(res == 2300);
  assert (seeds_to_distribute["test"] == 4000);

 //=====================================================

  succ.CreateHabSuit(false , "othersoil");
  for (int i = 0; i<10; i++) succ.DoSuccession({{"test", 100}}); //add 100 seeds and do succession
  //test DoSuccession with invalid CreateHabSuit setter

//restore RNG state.
//this is neceesary because the RNG was added to Cell::UpdateEnvSuitRefVal() after writing 
//various tests. Now that the drawing of two additional numbers changes the RNG state for 
//for all tests further down (line 488, plant model).
RNGs::mersenne = temp;


//further tests to write for parametrization: 
//1) assuming infinite seeds coming in from outside
// a cell with a single, entirely shade-tolerant PFG should grow to gsp.MaxAbund[High/Med/Low] * PFG.noStrata; 
// a PFG that is not shade-tolerant should have predictably cycling abundances. check and document behavior here (I assume max is gsp.MaxAbund * 1, sometimes up to 2*maxAbund)
//a cell with two identical, entirely shade-tolerant PFGs should have twice the abundance
//a cell with two identical, non shade-tolerant PFGs should get a more complex pattern, but total abundance should also be maxabund *1 to maxabund * 2. 
//a cell with many PFGs should usually have  total biomass x = sum(gsp.MaxAbund[i]) for all i which reach the highest stratum

//2) the cell is self-sufficient, only locally produced seeds are introduced. 
//total abundance differs between shade-tolerant and non-tolerant plants, depends further on growth pattern and lifespan

//test Doscussion with valid CreateHabsuit and 1 Disturbance
//test Doscussion with valid CreateHabsuit and 2 Disturbances

LOG(INFO) << "---Create GSP_base---";
  GSP_BASE gspb(default_WD + "/test.json");
  assert(gspb.simulDuration == 5);
  assert(gspb.doesSoil == true);
  assert(gspb.doesPlants == true);
  assert(gspb.doesAnimals == false);
  assert(gspb.doesManagement == true);
  assert(gspb.m_saveYears.size() == 3);
  assert(gspb.m_saveYears[0] == 1);
  assert(gspb.m_saveYears[1] == 2);
  assert(gspb.m_saveYears[2] == 4);


LOG(INFO) << "Create DATA_BASE";
  Data_BASE db(default_WD + "/test.json", gspb);
  assert(db.inputDir == "999-test/");
  assert(db.savingDir == "test-RESULTS/");
  assert(db.animalModelFile == "test.json");
  assert(db.plantModelFile == "test.json");
  assert(db.soilModelFile == "test.json");
  assert(db.listPlantFunctionalGroups.size() == 2 && 
    db.listPlantFunctionalGroups[0] == "generalist_grass" && 
    db.listPlantFunctionalGroups[1] == "generalist_tree");
  assert(db.listAnimalFunctionalGroups.size() == 10); 
  // && db.listAnimalFunctionalGroups[4] == "SIZE_V_1.2"); WARNING: become alphabetically sorted!
  assert(db.keyList.at("(1, 0, 1)") == 0);
  assert(db.keyList.getTotncell() == 18);
  assert(db.management.at("(1, 0, 1)")["fire"] == 0.5);
  assert(db.checkKeys(db.keyList.getKeys(), gspb));

LOG(INFO) << "Create Inputs class";
  Inputs_G inputs(default_WD + "/test.json");
  assert(inputs.checkKeys(db.keyList.getKeys()));

LOG(INFO) << "Create GSP_PLANTS";
  GSP_PLANTS gsp_plants(default_WD + "/test.json");
  assert(gsp_plants.m_saveYears[1] == 2);
  assert(gsp_plants.doesShadingPercentages);
  assert(gsp_plants.doesHabSuitability);
  assert(gsp_plants.doesSoilClass);
  assert(gsp_plants.doesSoilDepth);
  assert(gsp_plants.doesPlantDispersal);
  assert(gsp_plants.doesDisturbance);

  assert(gsp_plants.potentialFecundity == 1);
  assert(gsp_plants.noStrata == 4);
  assert(gsp_plants.maxAbundLow == 1000);
  assert(gsp_plants.maxAbundMedium == 1000);
  assert(gsp_plants.maxAbundHigh == 5000);
  assert(gsp_plants.seedInput == 100);
  assert(gsp_plants.lightThreshLow == 8000);
  assert(gsp_plants.lightThreshMedium == 5000);

LOG(INFO) << "Create DATA_PLANTs";
  Data_PLANTS data_plants(default_WD + "/test.json", gsp_plants);
  assert(data_plants.listPlantFunctionalGroups.size() == 2 && 
    data_plants.listPlantFunctionalGroups[0] == "generalist_grass" && 
    data_plants.listPlantFunctionalGroups[1] == "generalist_tree");
  assert(data_plants.shading.at("(1, 0, 1)") == 0.5);
  assert(data_plants.soilDepth.at("(1, 0, 1)") == 20);
  assert(data_plants.PFGDefinitions.size() == 2);
  assert(data_plants.PFGDefinitions.getPFG("generalist_grass").name == "generalist_grass");
  assert(data_plants.checkKeys(db.keyList.getKeys(), gsp_plants));

  Inputs_P inputs_p(default_WD + "/test.json");
  assert(inputs_p.checkKeys(db.keyList.getKeys()));

  
LOG(INFO) << "---Create a Plant model---";
  PlantModel plants(default_WD + "/test.json", db.keyList.getKeys());  //cell (1,0,1) has 0.5 shading, 20 depth; r[random number between 0 and 100] = 10 
  //-> create 10 x 0-1 + 10 seeds in cell 1,0,1
  plants.initialize(2); // 0 regional seeds, no disturbance, no light problems 
  //step 1 -> 10 1-2 year olds, 10 0-year olds from the 10 seeds. The 200 newly created seeds are dsitriubted to other cells, 1080 come from other cells
  //step 2 -> 10 1-3 year olds, 1080 0 year olds
  Landscape<double> results_grass =  plants.getPFGabund(0); //divides number by 10,000. (1080 + 30) / 10,000 = 0.111
  assert(results_grass.at("(1, 0, 1)") == 0.111);
  assert(results_grass.at("(0, 0, 0)") == 0.1178);

  //Create map with 100% disturbance in cell 1,0,1 (all individuals die), no disturbance in (0,0,0)
  Landscape<std::map<std::string, double>> testdisturbance(db.keyList.getKeys());
  std::map<std::string, double> filler{{"fire", 1.0}};
  std::map<std::string, double> filler2{{"fire", 0.0}};
  testdisturbance.fillWith(filler);
  testdisturbance.setValue("(0, 0, 0)", filler2);
  Landscape<std::string> validSoils(db.keyList.getKeys());
  validSoils.fillWith("SaLo_l_2");
  plants.createInputMaps(validSoils, testdisturbance, true);
  plants.TPlusOne_JJ();
  results_grass =  plants.getPFGabund(0);
  assert(results_grass.at("(1, 0, 1)") == 0);
  assert(results_grass.at("(0, 0, 0)") == 0.2038); //unaffected

  //make habitat unsuitable due to soil class. But do not kill plants yet (only offspring would be killed)
  validSoils.fillWith("invalidSoil");
  testdisturbance.fillWith(filler2); //no disturbance
  plants.createInputMaps(validSoils, testdisturbance, false);
  plants.TPlusOne_JJ();
  results_grass =  plants.getPFGabund(0);
  assert(results_grass.at("(0, 0, 0)") == 0.2038); //plants aged but no new seeds grow
  plants.TPlusOne_JJ();
  results_grass =  plants.getPFGabund(0);
  assert(results_grass.at("(0, 0, 0)") == 0.1945); //oldest cohort died

  //try again but this time kill all plants for whose habitat is unsuitable
  plants.createInputMaps(validSoils, testdisturbance, true);
  plants.TPlusOne_JJ();
  results_grass =  plants.getPFGabund(0);
  assert(results_grass.at("(0, 0, 0)") == 0); //this time they die


  //Vikys string-to-vector conversion (written by JJ)
  std::string test{"(0, 0, 0)"};
  std::vector <int> functionres = generalFunctions::stringToVector(test);
  assert(functionres[0] == 0 && functionres[1]==0 && functionres[2] == 0);
  test = "(0, 1, 0)";
  functionres = generalFunctions::stringToVector(test);
  assert(functionres[0] == 0 && functionres[1]==1 && functionres[2] == 0);
  test = "(0, 12, 7272)";
  functionres = generalFunctions::stringToVector(test);
  assert(functionres[0] == 0 && functionres[1]==12 && functionres[2] == 7272);
  test = "(4, 2, 0)";
  functionres = generalFunctions::stringToVector(test);
  assert(functionres[0] == 4 && functionres[1]==2 && functionres[2] == 0);
  test = "(4, 2, 0, 1)";
  functionres = generalFunctions::stringToVector(test);
  assert(functionres[0] == 4 && functionres[1]==2 && functionres[2] == 0 && functionres[3] == 1);
  test = "(3, 2)";
  functionres = generalFunctions::stringToVector(test);
  assert(functionres[0] == 3 && functionres[1]==2 && functionres.size() == 2);
}