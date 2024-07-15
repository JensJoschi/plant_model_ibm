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

// This file includes unit tests for the class Illumination. See tests/CMake file 
// and google test documentation.

#include "Illumination.h"
#include "Traits.h"
#include "Individual.h"
#include "Community.h"
#include "rng.h"


/** @cond */
#include "gtest/gtest.h"
#include <memory> 
/** @endcond */

class IlluminationTest : public ::testing::Test {
	protected:
	std::unique_ptr<Community> c;
	static const Traits* traits;
	static nlohmann::json individuals;
	std::shared_ptr<Soil> soil;	
	static std::vector<Stratum> strata;
	Illumination* illu;

	static void SetUpTestSuite() { //create species definnitions and 3 individuals
		nlohmann::json specTraits = {
			{"minDepth", 10},
			{"acceptedSoils", {{"sand", true}, {"clay", false}}},
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
			{"MaxHeight", 8},

			{"density", 2.0},
			{"name", "species"}
		};
		traits = new Traits(specTraits);
		nlohmann::json oneIndividual = {
			{"currGrowth",{
				{"height", 3.2},
				{"age", 4}
			}},
			{"currRes",{
				{"resources",  9999.0},
				{"biomass", 200.0}
				}
			},
			{"species", "species"}
		};
		individuals = {
			{"Hans", oneIndividual},
			{"John", oneIndividual},
		};
		oneIndividual["species"] = "anotherSpecies";
		individuals["Jill"] = oneIndividual;

		strata.push_back({0, 1, 150});
        strata.push_back({1, 2, 150});
        strata.push_back({2, 3, 150});
        strata.push_back({3, 4, 150});

		RNGs::mersenne = std::mt19937{2230};
	}


	void SetUp() override{
		soil = std::make_shared<Soil>(10, 10, "sand");
		std::map<std::string_view, const Traits*> traitlist;
		traitlist["species"] = traits;
		traitlist["anotherSpecies"] = traits;
		c = std::make_unique<Community>(soil, traitlist, 10, individuals);
		illu = new Illumination(strata, 1, 0.1);
	}

	void TearDown() override{
		c.reset();
		delete illu;
	}
	static void TearDownTestSuite() {
		delete traits;
		traits = nullptr;
	}
};

const Traits* IlluminationTest::traits = nullptr;
nlohmann::json IlluminationTest::individuals = nlohmann::json::array();
std::vector<Stratum> IlluminationTest::strata = std::vector<Stratum>();


TEST_F(IlluminationTest, builds){
	EXPECT_TRUE(c);
}

TEST_F(IlluminationTest, distribute){
	//tests if distribution of plant material is correct when all plants fit into the voxel
	//tests also correctness when plant exceeds voxel height
	illu->distributeIndividuals(*c);
	double res = illu->getPlantCover();
	double expectedBiomass =0.0;
	for (auto& ind: individuals){
		expectedBiomass += static_cast<double>(ind["currRes"]["biomass"]);
	}
	EXPECT_EQ(res, expectedBiomass / 2.0); //corrected for density
	EXPECT_EQ(res, c->getBiomass() / 2.0 );

	illu->clearIndividuals();
	illu->distributeIndividuals(*c);
	res = illu->getPlantCover();
	EXPECT_EQ(res, expectedBiomass / 2.0); 


	//individuals higher than a voxel
	c.reset();
	nlohmann::json largeIndividual = {
		{"currGrowth",{
			{"height", 8.0}, //twice as large as all strata combined
			{"age", 4}
		}},
		{"currRes",{{"resources",  9999.0},{"biomass", 200.0}
		}},{"species", "species"}
	};
	nlohmann::json ind{{"Foo", largeIndividual}};
	std::map<std::string_view, const Traits*> traitlist;
	traitlist["species"] = traits;
	c = std::make_unique<Community>(soil, traitlist, 10, ind);
	illu->clearIndividuals();
	illu->distributeIndividuals(*c);
	res = illu->getPlantCover();
	EXPECT_EQ(res, (200/2.0)/2.0); //half of the plant grows into this voxel.
}

TEST_F(IlluminationTest, distributeNeighbourSmall){
	//checks that no plants are distributed to a neighbour when all plants fit in a voxel
	Illumination neighbour(strata, 1, 0.1);
	illu->addNeighbours({&neighbour});
	illu->distributeIndividuals(*c);
	double res = illu->getPlantCover();
	EXPECT_EQ(res, c->getBiomass() / 2.0 ); //no change to test "distribute"
	EXPECT_EQ(neighbour.getPlantCover(), 0.0);
}

TEST_F(IlluminationTest, distributeNeighbourMultipleSmall){
	//when there are many plants that each fit in a voxel, 
	//1) no plant spills over to the neighbour,
	//2) the total amount of plant material is the sum of all individuals, 
	//   even if exceeding the voxel capacity
	//(not all of them will receive light though - but this is not tested here)
	c.reset();
	nlohmann::json ind{
		{"1", individuals["Hans"]},
		{"2", individuals["Hans"]},
		{"3", individuals["Hans"]},
		{"4", individuals["Hans"]},
		{"5", individuals["Hans"]},
		{"6", individuals["Hans"]},
		{"7", individuals["Hans"]},
		{"8", individuals["Hans"]},
		{"9", individuals["Hans"]},
		{"10", individuals["Hans"]}
	};
	std::map<std::string_view, const Traits*> traitlist;
	traitlist["species"] = traits;
	c = std::make_unique<Community>(soil, traitlist, 10, ind);
	EXPECT_EQ(c->getBiomass(), 2000.0);

	Illumination neighbour(strata, 1, 0.1);
	illu->addNeighbours({&neighbour});
	illu->distributeIndividuals(*c);
	double res = illu->getPlantCover();
	EXPECT_EQ(res, 2000.0/2.0); //which is more than the stratum can support
	EXPECT_EQ(neighbour.getPlantCover(), 0.0);
}

TEST_F(IlluminationTest, distributeNeighbourLarge){
	//tests that the correct amount of plant material spills over to neighbouring cell if plant is large
	c.reset();
	nlohmann::json largeIndividual = {
		{"currGrowth",{
			{"height", 1.0},
			{"age", 4}
		}},
		{"currRes",{
			{"resources",  9999.0},
			{"biomass", 500.0} //surface area = 250. Each stratum can only support 150--> 100 spill over
			}
		},
		{"species", "species"}
	};
	nlohmann::json ind{{"Foo", largeIndividual}};
	std::map<std::string_view, const Traits*> traitlist;
	traitlist["species"] = traits;
	c = std::make_unique<Community>(soil, traitlist, 10, ind);
	EXPECT_EQ(c->getBiomass(), 500.0);
	// without a neighbour: 
	// note: I added an assertion that every cell has a neighbour. 
	// This is not strictly needed and will be removed in the future. 
	// until this assertion is removed, this part will fail, so it is commented out. 
	// illu->distributeIndividuals(*c);
	// double res = illu->getPlantCover();
	// EXPECT_EQ(res, 150.0);
	// illu->clearIndividuals();

	// with a neighbour:
	Illumination neighbour(strata, 1, 0.1);
	illu->addNeighbours({&neighbour});
	illu->distributeIndividuals(*c);
	double res = illu->getPlantCover();
	EXPECT_EQ(res, 150.0);
	EXPECT_EQ(neighbour.getPlantCover(), 100.0);
}

TEST_F(IlluminationTest, distributeNeighbourExcessivelyLarge){
	//checks that plant material correctly spills over to neighobur and neighbour's 
	//neighbour if plant is large enough
	c.reset();
	nlohmann::json largeIndividual = {
		{"currGrowth",{
			{"height", 1.0},
			{"age", 4}
		}},
		{"currRes",{
			{"resources",  9999.0},
			{"biomass", 800.0} //400 area, spills over twice
			}
		},
		{"species", "species"}
	};
	nlohmann::json ind{{"Foo", largeIndividual}};
	std::map<std::string_view, const Traits*> traitlist;
	traitlist["species"] = traits;
	c = std::make_unique<Community>(soil, traitlist, 10, ind);
	EXPECT_EQ(c->getBiomass(), 800.0);
	Illumination neighbour1(strata, 1, 0.1);
	Illumination neighbour2(strata, 1, 0.1);
	illu->addNeighbours({&neighbour1});
	neighbour1.addNeighbours({&neighbour2});
	illu->distributeIndividuals(*c);
	EXPECT_EQ(illu->getPlantCover(), 150.0);
	EXPECT_EQ(neighbour1.getPlantCover(), 150.0);
	EXPECT_EQ(neighbour2.getPlantCover(), 100.0);
}

TEST_F(IlluminationTest, sendLightBeam){
	//checks that a single plant dies if not receiving enough light, and survives if receiving barely enough.
	//a plant with 200 biomass and 10% maintenance costs requires at least 20 light units to survive.
	//the light needs to actually hit the plant, and with a surface area of 100 and a stratum size of 150, only 2/3 
	//of the light will hit the plant.Thus, more than 30 light is required to survive.
	illu = new Illumination(strata, 1, 0.0);
	nlohmann::json oneIndividual = {
			{"currGrowth",{
				{"height", 1.0},
				{"age", 4}
			}},
			{"currRes",{
				{"resources",  0.0},
				{"biomass", 200.0}
				}
			},
			{"species", "species"}
	};
	nlohmann::json ind{{"Foo", oneIndividual}};
	std::map<std::string_view, const Traits*> traitlist;
	traitlist["species"] = traits;
	c = std::make_unique<Community>(soil, traitlist, 0, ind);
	//no light here
	c->age();
	EXPECT_EQ(c->getCount(), 0);

	c.reset();
	c = std::make_unique<Community>(soil, traitlist, 0, ind);
	illu->distributeIndividuals(*c);
	illu->sendLightBeam(30);
	c->age();
	EXPECT_EQ(c->getCount(), 0);//barely not enough, resources reach 0.

	c.reset();
	illu->clearIndividuals();
	c = std::make_unique<Community>(soil, traitlist, 0, ind);
	illu->distributeIndividuals(*c);
	illu->sendLightBeam(31);
	c->age();
	EXPECT_EQ(c->getCount(), 1);
}

TEST_F(IlluminationTest, highPlant){
	//tests that a single plant dies if not receiving enough light, and survives if receiving barely enough.
	//here the plant covers 2 strata, thus partially shading itself. 
	//with 400 biomass, it requires >40 light to survive. With 100 area per stratum it again covers 2/3 of each stratum. 
	//x*2/3 + x* 1/3 * 2/3 = 41
	//x = 46.125 should be the required light input
	illu = new Illumination(strata, 1, 0.0);
	nlohmann::json oneIndividual = {
		{"currGrowth",{
			{"height", 2.0}, //grows into 2 strata
			{"age", 4}
		}},
		{"currRes",{
			{"resources",  0.0},
			{"biomass", 400.0}
			}
		}, {"species", "species"}
	};
	nlohmann::json ind{{"Foo", oneIndividual}};
	std::map<std::string_view, const Traits*> traitlist;
	traitlist["species"] = traits;
	c = std::make_unique<Community>(soil, traitlist, 0, ind);
	illu->distributeIndividuals(*c);
	illu->sendLightBeam(45);
	c->age();
	EXPECT_EQ(c->getCount(), 0);
	c.reset();

	illu->clearIndividuals();
	c = std::make_unique<Community>(soil, traitlist, 0, ind);
	illu->distributeIndividuals(*c);
	illu->sendLightBeam(47);
	c->age();
	EXPECT_EQ(c->getCount(), 1);

	//what about 46 light?
	//46 *2/3 = 30.66; 46*1/3 = 15.33. 
	//15* 1/3 = 10; 31 + 10 = 41. 
	// The rounding to 31 causes slight overestimation and makes the plant survive
}

TEST_F(IlluminationTest, twoEquals){
	//two plants that each would fit in the voxel together overfill it. 
	//checks that only one of the two survives
	illu = new Illumination(strata, 1, 0.0);
	c.reset();
	nlohmann::json oneIndividual = {
		{"currGrowth",{
			{"height", 1.0},
			{"age", 4}
		}},
		{"currRes",{
			{"resources",  0.0},
			{"biomass", 300.0}
			}
		},
		{"species", "species"}
	};
	nlohmann::json anotherIndividual = {
		{"currGrowth",{
			{"height", 1.0},
			{"age", 4}
		}},
		{"currRes",{
			{"resources",  0.0},
			{"biomass", 300.0}
		}
		},
		{"species", "anotherSpecies"} //actually the same traits, but used here to distinguish the two
	};
	nlohmann::json ind{{"Foo", oneIndividual}, {"Bar", anotherIndividual}};
	std::map<std::string_view, const Traits*> traitlist;
	traitlist["species"] = traits;
	traitlist["anotherSpecies"] = traits;
	c = std::make_unique<Community>(soil, traitlist, 0, ind);
	illu->distributeIndividuals(*c);
	illu->sendLightBeam(31); //enough for only one of them.
	c->age();
	EXPECT_EQ(c->getCount(), 1);
	// EXPECT_EQ(c->getCount("species"), 1);
	//RNG is fixed, but std::uniform_distribution implementation is OS-specific. 
	//Leads to different results on different OS.
}

TEST_F(IlluminationTest, twoSmallEquals){
	//two plants together do not overfill a voxel. 
	//checks that both survive when receiving barely enough light.
	illu = new Illumination(strata, 1, 0.0);
	c.reset();
	nlohmann::json oneIndividual = {
		{"currGrowth",{
			{"height", 1.0},
			{"age", 4}
		}},
		{"currRes",{
			{"resources",  0.0},
			{"biomass", 150.0}
			}
		},
		{"species", "species"}
	};
	nlohmann::json anotherIndividual = {
		{"currGrowth",{
			{"height", 1.0},
			{"age", 4}
		}},
		{"currRes",{
			{"resources",  0.0},
			{"biomass", 150.0}
		}
		},
		{"species", "anotherSpecies"}
	};
	nlohmann::json ind{{"Foo", oneIndividual}, {"Bar", anotherIndividual}};
	std::map<std::string_view, const Traits*> traitlist;
	traitlist["species"] = traits;
	traitlist["anotherSpecies"] = traits;
	c = std::make_unique<Community>(soil, traitlist, 0, ind);
	illu->distributeIndividuals(*c);
	illu->sendLightBeam(31); //note: light should not be interpreted as "number of photons" 
	//but as watt/m^2. 31 is enough for one, or for two, or 10, as long as the watt/surface area is sufficient for each plant.
	//that is true if each plant's surface area is large enough given energy input, and if the plants do not overlap.
	c->age();
	EXPECT_EQ(c->getCount(), 2);
}

TEST_F(IlluminationTest, oneLarger){
	//two plants with different heights, one shades the other. 
	//checks that only the larger plant survives. 
	illu = new Illumination(strata, 1, 0.0);
	c.reset();
	nlohmann::json oneIndividual = {
		{"currGrowth",{
			{"height", 2.0},
			{"age", 4}
		}},
		{"currRes",{
			{"resources",  0.0},
			{"biomass", 600.0}
			}
		},
		{"species", "species"}
	};
	nlohmann::json anotherIndividual = {
		{"currGrowth",{
			{"height", 1.0},
			{"age", 4}
		}},
		{"currRes",{
			{"resources",  0.0},
			{"biomass", 300.0}
		}
		},
		{"species", "anotherSpecies"}
	};
	nlohmann::json ind{{"Foo", oneIndividual}, {"Bar", anotherIndividual}};
	std::map<std::string_view, const Traits*> traitlist;
	traitlist["species"] = traits;
	traitlist["anotherSpecies"] = traits;
	c = std::make_unique<Community>(soil, traitlist, 0, ind);
	illu->distributeIndividuals(*c);
	illu->sendLightBeam(62);
	c->age();
	EXPECT_EQ(c->getCount(), 1);
	EXPECT_EQ(c->getCount("species"), 1);
}


TEST_F(IlluminationTest, oneLargerInfiniteLight){
	//two plants with different heights, one shades the other and completely blocks incoming light
	//checks that only the larger plant survives even if light is not limited.
	illu = new Illumination(strata, 1, 0.0);
	c.reset();
	nlohmann::json oneIndividual = {
		{"currGrowth",{
			{"height", 2.0},
			{"age", 4}
		}},
		{"currRes",{
			{"resources",  0.0},
			{"biomass", 600.0}
			}
		},
		{"species", "species"}
	};
	nlohmann::json anotherIndividual = {
		{"currGrowth",{
			{"height", 1.0},
			{"age", 4}
		}},
		{"currRes",{
			{"resources",  0.0},
			{"biomass", 300.0}
		}
		},
		{"species", "anotherSpecies"}
	};
	nlohmann::json ind{{"Foo", oneIndividual}, {"Bar", anotherIndividual}};
	std::map<std::string_view, const Traits*> traitlist;
	traitlist["species"] = traits;
	traitlist["anotherSpecies"] = traits;
	c = std::make_unique<Community>(soil, traitlist, 0, ind);
	illu->distributeIndividuals(*c);
	illu->sendLightBeam(100000);
	c->age();
	EXPECT_EQ(c->getCount(), 1);
	EXPECT_EQ(c->getCount("species"), 1);
}

TEST_F (IlluminationTest, diffusion){
	//two plants with different heights, one shades the other. 
	//checks that lower plant receives light when light diffuses upon hitting a plant
	c.reset();
	nlohmann::json oneIndividual = {
		{"currGrowth",{
			{"height", 2.0},
			{"age", 4}
		}},
		{"currRes",{
			{"resources",  0.0},
			{"biomass", 600.0}
			}
		},
		{"species", "species"}
	};
	nlohmann::json anotherIndividual = {
		{"currGrowth",{
			{"height", 1.0},
			{"age", 4}
		}},
		{"currRes",{
			{"resources",  0.0},
			{"biomass", 300.0}
		}
		},
		{"species", "anotherSpecies"}
	};
	nlohmann::json ind{{"Foo", oneIndividual}, {"Bar", anotherIndividual}};
	std::map<std::string_view, const Traits*> traitlist;
	traitlist["species"] = traits;
	traitlist["anotherSpecies"] = traits;

	RNGs::mersenne = std::mt19937{74};
	illu = new Illumination(strata, 5, 0.0);
	c = std::make_unique<Community>(soil, traitlist, 0, ind);
	illu->distributeIndividuals(*c);
	illu->sendLightBeam(90);
	c->age();
	EXPECT_EQ(c->getCount("species"), 1); //taller, gets the light
	EXPECT_EQ(c->getCount("anotherSpecies"), 0);

	c.reset();
	RNGs::mersenne = std::mt19937{74};
	illu = new Illumination(strata, 5, 0.9); //unrealistically high diffusion for testing
	c = std::make_unique<Community>(soil, traitlist, 0, ind);
	illu->distributeIndividuals(*c);
	illu->sendLightBeam(90); //10% of the light hit upper stratum, remaining 90% are diffused to lower stratum
	c->age();
	EXPECT_EQ(c->getCount("species"), 0); //roughly equal distribution of resources, but species requires twice as much (60)
	EXPECT_EQ(c->getCount("anotherSpecies"), 1);
	RNGs::mersenne = std::mt19937{2230};
}
