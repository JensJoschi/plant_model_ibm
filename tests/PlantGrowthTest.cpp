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
    nlohmann::json j = {
        {"MaturationTime", 5},
        {"LifeSpan", 10},
        {"MaxHeight", 100}};
    const LifeHistory traits{j};
    PlantGrowth p{&traits};
};

TEST_F(PlantGrowthTest, buildWithJson){
    nlohmann::json correctVariables = {
        {"height", 3.2},
        {"age", 4}};
    nlohmann::json heightMissing = {
        {"age", 4}};
    nlohmann::json ageMissing = {
        {"height", 3.2}};
    nlohmann::json wrongType = {
        {"height", "3.2"},
        {"age", 4}};
    nlohmann::json negativeHeight = {
        {"height", -3.2},
        {"age", 4}};
    nlohmann::json negativeAge = {
        {"height", 3.2},
        {"age", -4}};
    nlohmann::json ageExceedsLifespan = {
        {"height", 3.2},
        {"age", 11}};
    nlohmann::json heightExceedsMaxHeight = {
        {"height", 101.0},
        {"age", 4}};
    ASSERT_NO_THROW (PlantGrowth (&traits, correctVariables));
    ASSERT_ANY_THROW(PlantGrowth (&traits, heightMissing));
    ASSERT_ANY_THROW(PlantGrowth (&traits, ageMissing));
    ASSERT_ANY_THROW(PlantGrowth (&traits, wrongType));
    ASSERT_ANY_THROW(PlantGrowth (&traits, negativeHeight));
    ASSERT_ANY_THROW(PlantGrowth (&traits, negativeAge));
    ASSERT_ANY_THROW(PlantGrowth (&traits, ageExceedsLifespan));
    ASSERT_ANY_THROW(PlantGrowth (&traits, heightExceedsMaxHeight));
}

TEST_F(PlantGrowthTest, grow) {
    EXPECT_TRUE(p.getHeight() == 1);
    for (int i = 0; i < 5; i++) {
        EXPECT_TRUE(p.grow());
    }
    EXPECT_TRUE(p.getHeight() == 50);
    for (int i = 0; i < 5; i++) {
        EXPECT_TRUE(p.grow());
    }
    EXPECT_FALSE(p.grow());
    EXPECT_TRUE(p.getHeight() > 99.0 && p.getHeight() < 100.0);//asymptotic growth
}

