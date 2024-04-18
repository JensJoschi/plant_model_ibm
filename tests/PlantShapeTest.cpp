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

// This file includes unit tests for the class PlantShape and its inherited versions (part of the traits definitions). See tests/CMake file 
// and google test documentation.

#include "plantShape.h"
/** @cond */
#include "gtest/gtest.h"
#include "nlohmann/json.hpp"
#include "easylogging++.h"
/** @endcond */


class PlantShapeTest : public ::testing::Test {
protected:
    nlohmann::json validTraits = {
        {"density", 2.0}
    };
    nlohmann::json emptyTraits = {};
    nlohmann::json excessTraits = {
        {"density", 2.0},
        {"excess", 4.2123}
    };    
    nlohmann::json wrongType = {
        {"density", "2.0"}
    };
    nlohmann::json DensityInt = {
        {"density", 1000}
    };
    nlohmann::json DensityZero = {
        {"density", 0.0}
    };
    nlohmann::json DensityNegative = {
        {"density", -2.0}
    };

    void SetUp() override {
        el::Configurations conf("debuglog.conf");
        el::Loggers::reconfigureAllLoggers(conf); 
    }
};

TEST_F(PlantShapeTest, InputValid){
    ASSERT_NO_THROW(PlantShape p(validTraits));
}

TEST_F(PlantShapeTest, InputEmpty){
    ASSERT_THROW(PlantShape p(emptyTraits), nlohmann::json::exception);
}

TEST_F(PlantShapeTest, InputExcess){
    ASSERT_NO_THROW(PlantShape p(excessTraits));
}

TEST_F(PlantShapeTest, InputWrongType){
    ASSERT_THROW(PlantShape p(wrongType), nlohmann::json::exception);
}

TEST_F(PlantShapeTest, InputDensityInt){
    ASSERT_NO_THROW(PlantShape p(DensityInt));
}

TEST_F(PlantShapeTest, InputDensityZero){
    ASSERT_THROW(PlantShape p(DensityZero), std::invalid_argument);
}

TEST_F(PlantShapeTest, InputDensityNegative){
    ASSERT_THROW(PlantShape p(DensityNegative), std::invalid_argument);
}

TEST_F(PlantShapeTest, getArea){
    PlantShape p(validTraits);
    EXPECT_FLOAT_EQ(p.getArea(100), 50.0);
}

TEST_F(PlantShapeTest, getArea_Rect){
    PlantRectangle r(validTraits);
    EXPECT_FLOAT_EQ(r.PlantShape::getArea(100), 50.0);
    EXPECT_FLOAT_EQ(r.getArea(100, 10.0, 0, 10), 50.0);
    EXPECT_FLOAT_EQ(r.getArea(100, 20.0, 0, 10), 25.0);
    EXPECT_FLOAT_EQ(r.getArea(100, 20.0, 10, 20), 25.0);
    EXPECT_FLOAT_EQ(r.getArea(100, 20.0, 7, 7), 0.0);

}