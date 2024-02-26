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

// This file includes unit tests for the class PlantGrowth. See tests/CMake file 
// and google test documentation.

#include "PlantGrowth.h"
#include "LifeHistory.h"

/** @cond */
#include "gtest/gtest.h"
/** @endcond */

class PlantGrowthTest : public ::testing::Test {
    protected:
   protected:
    nlohmann::json j = {
        {"MaturationTime", 5},
        {"LifeSpan", 10},
        {"MaxHeight", 100}};
    nlohmann::json j2 = {
        {"MaturationTime", 5},
        {"LifeSpan", 10},
        {"MaxHeight", 3}};
    const LifeHistory traits{j};
    const LifeHistory traits2{j2};
    PlantGrowth p{&traits};
    PlantGrowth p2{&traits2};
};

TEST_F(PlantGrowthTest, age) {
    EXPECT_TRUE(p.getHeight() == 0);
    for (int i = 0; i < 5; i++) {
        EXPECT_TRUE(p.age());
    }
    EXPECT_TRUE(p.getHeight() == 50);
    for (int i = 0; i < 5; i++) {
        EXPECT_TRUE(p.age());
    }
    EXPECT_FALSE(p.age());
    EXPECT_TRUE(p.getHeight() == 99);//asymptotic growth
}

TEST_F(PlantGrowthTest, getProportion){
    EXPECT_TRUE(p.getHeight() == 0);
    for (int i = 0; i < 5; i++) {
        p.age();
    }
    EXPECT_TRUE(p.getHeight() == 50);
    EXPECT_FLOAT_EQ(p.getProportion(0, 50), 1.0);
    EXPECT_FLOAT_EQ(p.getProportion(0, 25), 0.5);
    EXPECT_FLOAT_EQ(p.getProportion(25, 50), 0.5);
    EXPECT_FLOAT_EQ(p.getProportion(0, 100), 1.0);
    EXPECT_FLOAT_EQ(p.getProportion(0, 23), 0.46);
    EXPECT_FLOAT_EQ(p.getProportion(2, 23), 0.42);
}