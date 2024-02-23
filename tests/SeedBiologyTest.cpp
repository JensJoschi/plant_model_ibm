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

// This file includes unit tests for the class SeedBiology (part of the traits definitions). See tests/CMake file 
// and google test documentation.

#include "SeedBiology.h"

/** @cond */
#include "gtest/gtest.h"
#include "nlohmann/json.hpp"
#include "easylogging++.h"
/** @endcond */

class SeedBiologyTest : public ::testing::Test {
protected:
    nlohmann::json validTraits = {
        {"Dormancy", false},
        {"GerminationSuccess", 0.1},
        {"MortalityDormant", 0.05},
        {"DormancyBreakRate", 0.8}
    };
    nlohmann::json emptyTraits = {};
    nlohmann::json missingTraits = {
        {"GerminationSuccess", 0.1}
    };
    nlohmann::json excessTraits = {
        {"Dormancy", false},
        {"GerminationSuccess", 0.1},
        {"MortalityDormant", 0.05},
        {"DormancyBreakRate", 0.8},
        {"excess", 4.2123}
    };
    nlohmann::json wrongTypeDormancy = {
        {"Dormancy", "true"},
        {"GerminationSuccess", 0.1},
        {"MortalityDormant", 0.05},
        {"DormancyBreakRate", 0.8}
    };
    nlohmann::json MortalityInt = {
        {"Dormancy", false},
        {"GerminationSuccess", 1},
        {"MortalityDormant", 0.05},
        {"DormancyBreakRate", 0.8}
    };
    nlohmann::json MortalityInt2 = {
        {"Dormancy", false},
        {"GerminationSuccess", 2},
        {"MortalityDormant", 0.05},
        {"DormancyBreakRate", 0.8}
    };
    nlohmann::json NegativeMortality = {
        {"Dormancy", false},
        {"GerminationSuccess", -0.1},
        {"MortalityDormant", 0.05},
        {"DormancyBreakRate", 0.8}
    };
    nlohmann::json ActivateLargerOne = {
        {"Dormancy", false},
        {"GerminationSuccess", 0.1},
        {"MortalityDormant", 0.05},
        {"DormancyBreakRate", 1.1}
    };
    nlohmann::json MortalityActiveSmallerDormant = {
        {"Dormancy", false},
        {"GerminationSuccess", 0.05},
        {"MortalityDormant", 0.1},
        {"DormancyBreakRate", 0.8}
    };

    void SetUp() override {
        el::Configurations conf("debuglog.conf");
        el::Loggers::reconfigureAllLoggers(conf); 
    }
};

TEST_F(SeedBiologyTest, ValidTraits) {
    ASSERT_NO_THROW(SeedBiology sb(validTraits));
}

TEST_F(SeedBiologyTest, EmptyTraits) {
    ASSERT_THROW(SeedBiology sb(emptyTraits), nlohmann::json::exception);
}

TEST_F(SeedBiologyTest, MissingTraits) {
    ASSERT_THROW(SeedBiology sb(missingTraits), nlohmann::json::exception);
}

TEST_F(SeedBiologyTest, ExcessTraits) {
    ASSERT_NO_THROW(SeedBiology sb(excessTraits));
}

TEST_F(SeedBiologyTest, WrongTypeDormancy) {
    ASSERT_THROW(SeedBiology sb(wrongTypeDormancy), nlohmann::json::exception);
}

TEST_F(SeedBiologyTest, MortalityActiveInt) {
    ASSERT_NO_THROW(SeedBiology sb(MortalityInt)); //vals of zero or 1 are valid
    ASSERT_THROW(SeedBiology sb(MortalityInt2), std::invalid_argument);
}

TEST_F(SeedBiologyTest, NegativeMortality) {
    ASSERT_THROW(SeedBiology sb(NegativeMortality), std::invalid_argument);
}

TEST_F(SeedBiologyTest, ActivateLargerOne) {
    ASSERT_THROW(SeedBiology sb(ActivateLargerOne), std::invalid_argument);
}

TEST_F(SeedBiologyTest, MortalityActiveSmallerDormant) {
    ASSERT_THROW(SeedBiology sb(MortalityActiveSmallerDormant), std::invalid_argument);
}

