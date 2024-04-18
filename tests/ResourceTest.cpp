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
 // Jens Joschinski (IBM)
 // --------------------------------------------------------------------------

// This file includes unit tests for the class Resource. See tests/CMake file 
// and google test documentation.

#include "PlantResource.h"

/** @cond */
#include "gtest/gtest.h"
#include "nlohmann/json.hpp"
/** @endcond */

class ResourceTest : public ::testing::Test {
    protected:
    nlohmann::json j = {
        {"conversionEfficiency", 1.0},
        {"maintenanceCosts", 0.1},
        {"seedAllocation", 0.01},
        {"biomassAllocation", 0.5},
        {"maxBiomass", 1000},
        {"shadeFactor", 0.5}};
    nlohmann::json j2 = {
        {"conversionEfficiency", 0.5},
        {"maintenanceCosts", 0.1},
        {"seedAllocation", 0.01},
        {"biomassAllocation", 0.5},
        {"maxBiomass", 1000},
        {"shadeFactor", 0.5}};
    const ResourceAlloc allocation{j};
    const ResourceAlloc allocation2{j2};
    PlantResource pr{&allocation};
    PlantResource pr2{&allocation2};
};

TEST_F(ResourceTest, buildWithJson){
    nlohmann::json goodParameters{
        {"resources", 100},
        {"biomass", 100}};
    nlohmann::json missingBiomass{
        {"resources", 100}};
    nlohmann::json missingResources{
        {"biomass", 100}};
    nlohmann::json wrongType{
        {"resources", "100"},
        {"biomass", 100}};
    nlohmann::json negativeResources{
        {"resources", -100},
        {"biomass", 100}};
    nlohmann::json negativeBiomass{
        {"resources", 100},
        {"biomass", -100}};
    nlohmann::json tooMuchBiomass{
        {"resources", 100},
        {"biomass", 1001}};
    
    ASSERT_NO_THROW(PlantResource(&allocation, goodParameters));
    ASSERT_ANY_THROW(PlantResource(&allocation, missingBiomass));
    ASSERT_ANY_THROW(PlantResource(&allocation, missingResources));
    ASSERT_ANY_THROW(PlantResource(&allocation, wrongType));
    ASSERT_ANY_THROW(PlantResource(&allocation, negativeResources));
    ASSERT_ANY_THROW(PlantResource(&allocation, negativeBiomass));
    ASSERT_ANY_THROW(PlantResource(&allocation, tooMuchBiomass));
}

TEST_F(ResourceTest, updateResource) {
    EXPECT_EQ(pr.resources, 0);
    pr.updateResource(0);
    EXPECT_EQ(pr.resources, 0);
    pr.updateResource(100);
    EXPECT_EQ(pr.resources, 100);
    pr2.updateResource(100);        //different object! half efficiency
    EXPECT_EQ(pr2.resources, 50); 
    pr2.updateResource(100, false); //soil is not suitable
    EXPECT_EQ(pr2.resources, 50);
}

TEST_F(ResourceTest, allocateResources) {
    std::pair<bool, int> seeds = pr.allocateResources(); //first: survival, second:no. of seeds
    EXPECT_EQ(seeds.first, false);//plant has no resources to spend
    EXPECT_EQ(seeds.second, 0);
    EXPECT_EQ(pr.getBiomass(), 100);

    nlohmann::json specificState{ 
        {"resources", 30},
        {"biomass", 100}};
    PlantResource r(&allocation, specificState);
    seeds = r.allocateResources();
    EXPECT_EQ(r.getBiomass(), 110);  //resource-limited. 10 maintenance costs, 20*alloc = 10 growth

    nlohmann::json specificState2{ 
        {"resources", 10000000},
        {"biomass", 100}};
    PlantResource r2(&allocation, specificState2);
    seeds = r2.allocateResources();
    EXPECT_EQ(seeds.second, 99999); //potentially infinite, only limited by resource (not 100000 because of maintenance costs)
    EXPECT_EQ(r2.getBiomass(), 130); //limited by square-root function. maxgrowth = 1000, current biomass = 100; 1000-100 = 900; sqrt(900) = 30
}

TEST_F(ResourceTest, disturb){
    EXPECT_EQ(pr.getBiomass(), 100);
    int deficit = pr.disturb(50);
    EXPECT_EQ(deficit, 0);
    EXPECT_EQ(pr.getBiomass(), 50);

    deficit = pr.disturb(60);
    EXPECT_EQ(deficit, 10);
    EXPECT_EQ(pr.getBiomass(), 0);

    deficit = pr.disturb(0);
    EXPECT_EQ(deficit, 0);
    EXPECT_EQ(pr.getBiomass(), 0);
}

TEST_F(ResourceTest, depleteResources){
    //0 resource at start (after constructing)
    EXPECT_EQ(pr.depleteResources(50), 50);
    pr.updateResource(40);
    EXPECT_EQ(pr.depleteResources(50), 10);
    pr.updateResource(100);
    EXPECT_EQ(pr.depleteResources(70), 0); //30 resources left
    std::pair<bool, int> seeds = pr.allocateResources(); 
    EXPECT_EQ(pr.getBiomass(), 110);//see test AllocateResources, specificState
}

TEST_F(ResourceTest, getBiomass){
    EXPECT_EQ(pr.getBiomass(), 100);
}