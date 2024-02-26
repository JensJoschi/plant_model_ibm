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

// This file includes unit tests for the class LifeHistory (part of the traits definitions). See tests/CMake file 
// and google test documentation.

#include "LifeHistory.h"

/** @cond */
#include "gtest/gtest.h"
#include "nlohmann/json.hpp"
#include "easylogging++.h"
/** @endcond */


class LifeHistoryTest : public ::testing::Test {
protected:
    nlohmann::json validTraits = {
        {"MaturationTime", 5},
        {"LifeSpan", 10},
        {"MaxHeight", 100},
    };
    nlohmann::json emptyTraits = {};
    nlohmann::json missingTraits = {
        {"MaturationTime", 5},
        {"MaxHeight", 100},
    };
    nlohmann::json excessTraits = {
        {"MaturationTime", 5},
        {"LifeSpan", 10},
        {"MaxHeight", 100},
        {"Excess", 0.5}
    };
    nlohmann::json wrongTypeMatTime = {
        {"MaturationTime", "5"},
        {"LifeSpan", 10},
        {"MaxHeight", 100},
    };
    nlohmann::json MatTimeFloat = {
        {"MaturationTime", 5.5},
        {"LifeSpan", 10},
        {"MaxHeight", 100},
    };
    nlohmann::json NegativeMat = {
        {"MaturationTime", -5},
        {"LifeSpan", 10},
        {"MaxHeight", 100},
    };
    nlohmann::json NegativeHeight = {
        {"MaturationTime", 5},
        {"LifeSpan", 10},
        {"MaxHeight", -100},
    };
    nlohmann::json MatLargerLife = {
        {"MaturationTime", 15},
        {"LifeSpan", 10},
        {"MaxHeight", 100},
    };

    void SetUp() override {
        el::Configurations conf("debuglog.conf");
        el::Loggers::reconfigureAllLoggers(conf); 
    }
};

TEST_F(LifeHistoryTest, ValidTraits) {
    ASSERT_NO_THROW(LifeHistory l(validTraits));
}
TEST_F(LifeHistoryTest, EmptyTraits) {
    ASSERT_THROW(LifeHistory l(emptyTraits), nlohmann::json::exception);
}
TEST_F(LifeHistoryTest, MissingTraits) {
    ASSERT_THROW(LifeHistory l(missingTraits), nlohmann::json::exception);
}
TEST_F(LifeHistoryTest, ExcessTraits) {
    ASSERT_NO_THROW(LifeHistory l(excessTraits));
}
TEST_F(LifeHistoryTest, WrongType) {
    ASSERT_THROW(LifeHistory l(wrongTypeMatTime),  std::runtime_error);
}
TEST_F(LifeHistoryTest, MatTimeFloat) {
    ASSERT_THROW(LifeHistory l(MatTimeFloat), std::runtime_error);
}
TEST_F(LifeHistoryTest, NegativeVals) {
    ASSERT_THROW(LifeHistory l(NegativeMat), std::invalid_argument);
    ASSERT_THROW(LifeHistory l(NegativeHeight), std::invalid_argument);
}
TEST_F(LifeHistoryTest, MatLargerLife) {
    ASSERT_THROW(LifeHistory l(MatLargerLife), std::invalid_argument);
}



