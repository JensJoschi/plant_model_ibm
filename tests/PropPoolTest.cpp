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

// This file includes unit tests for the class PropPool. See tests/CMake file 
// and google test documentation.

#include "PropPool.h"

/** @cond */
#include "gtest/gtest.h"
/** @endcond */

  //  This class needs a rewrite. See comments in propPool.h

class PropPoolTest : public ::testing::Test {
protected:
    PropPool p{100,true,1};

    // void SetUp() override {
    //     p = PropPool(100, true, 1); //creates a seed mass of 100, which declines over time due to mortality. 
    // }
};

TEST_F(PropPoolTest, InitialSizeIsCorrect) {
    EXPECT_EQ(p.getSize(), 100);
}

TEST_F(PropPoolTest, AgePoolReducesSizeByHalf) {
    p.AgePool1(1);
  //the value "1" describes something like the half-life of the seed mass, a value of 1 causes 50% mortality. 
    EXPECT_EQ(p.getSize(), 50);
}

TEST_F(PropPoolTest, AgePoolL5) {
    p.AgePool1(5); //half life is 5 years
    EXPECT_EQ(p.getSize(), 83);
    for(int i = 0; i<3; i++){
        p.AgePool1(5);
    }
    EXPECT_EQ(p.getSize(), 47); //formula is not accurate. See PropPool.h
}

TEST_F(PropPoolTest, PutSeedInPoolDoesNotReplaceSmallerPool) {
    p.AgePool1(1); 
    p.PutSeedInPool(37);//new seed pool is smaller, so it is not replaced. weird behavior, see propPool.h
    EXPECT_EQ(p.getSize(), 50);
}

TEST_F(PropPoolTest, PutSeedInPoolReplacesLargerPool) {
    p.AgePool1(1);
    p.PutSeedInPool(137);//new seed pool is larger, so it is replaced. weird behavior, see propPool.h
    EXPECT_EQ(p.getSize(), 137);
}

