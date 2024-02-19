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
 // Jens Joschinski
 // --------------------------------------------------------------------------

// This file includes unit tests for the class Resource. See tests/CMake file 
// and google test documentation.

#include "PlantResource.h"
#include "gtest/gtest.h"

class ResourceTest : public ::testing::Test {
    protected: 
    const ResourceAlloc allocation;
    const ResourceAlloc allocation2{0.5, 0.01, 0.5, 0.1, 0.05};
    PlantResource pr{&allocation};
    PlantResource pr2{&allocation2};
};

TEST_F(ResourceTest, updateResource) {
    EXPECT_EQ(pr.updateResource(0), 0);
    EXPECT_EQ(pr.updateResource(100), 100); //assuming conversion efficiency == 1
    EXPECT_EQ(pr2.updateResource(100), 50); //assuming conversion efficiency == 0.5

}

TEST_F(ResourceTest, isCritical) {
    EXPECT_TRUE(pr.isResourceCritical());
    pr.updateResource(100);
    EXPECT_FALSE(pr.isResourceCritical());
}

TEST_F(ResourceTest, allocateResources) {
    Allocations a = pr.allocateResources(100); //resources need to feed 100 biomass
    //plant contains no resources yet
    EXPECT_EQ(a.seeds, 0); 
    EXPECT_EQ(a.biomass, 0);

    pr.updateResource(100);
    a = pr.allocateResources(100);
     //maxinvestment is 0.05, so 100 * 0.05 = 5
    EXPECT_EQ(a.seeds, 0); 
    EXPECT_EQ(a.biomass, 5); 
    
    a = pr.allocateResources(21); 
    //5% of the biomass again
    EXPECT_EQ(a.seeds, 0);
    EXPECT_EQ(a.biomass, 1);

    a = pr.allocateResources(21000000); 
    //biomass so large that maintenance over-depletes resources
    EXPECT_TRUE(pr.isResourceCritical());
    EXPECT_EQ(a.seeds, 0);
    EXPECT_EQ(a.biomass, 0);
    
    pr2.updateResource(10000000); 
    a = pr2.allocateResources(1000);
    //nearly infinite resources, large biomass
    //note that conversion efficiency = 0.5, so resource is half of the light input
    EXPECT_EQ(a.biomass, 50); //max investment
    EXPECT_EQ(a.seeds, 49999); //resource-limited only (seed allocation after maintenance costs)
}