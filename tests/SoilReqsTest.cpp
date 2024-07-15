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

// This file includes unit tests for the class SoilRequirements (part of the traits definitions). See tests/CMake file 
// and google test documentation.

#include "SoilRequirements.h"

/** @cond */
#include "gtest/gtest.h"
#include "nlohmann/json.hpp"
#include "easylogging++.h"
/** @endcond */

class SoilRequirementsTest : public ::testing::Test {
protected:
    nlohmann::json validTraits = {
        {"minDepth", 10},
        {"acceptedSoils", {{"sand", true}, {"clay", false}}},
        {"size", 1}
    };
    nlohmann::json emptyTraits = {};
    nlohmann::json missingTraits = {
        {"minDepth", 10},
        {"size", 1}
    };
    nlohmann::json excessTraits = {
        {"minDepth", 10},
        {"acceptedSoils", {{"sand", true}, {"clay", false}}},
        {"excess", 4.2123},
        {"size", 1}
    };
    nlohmann::json wrongTypeDepth = {
        {"minDepth", 10.21},
        {"acceptedSoils", {{"sand", true}, {"clay", false}}},
        {"size", 1}
    };
    nlohmann::json wrongTypeSoil = {
        {"minDepth", 10},
        {"acceptedSoils", {{"sand", 1}, {"clay", false}}},
        {"size", 1}
    };
    nlohmann::json emptySoilTraits = {
        {"minDepth", 10},
        {"acceptedSoils", {}},
        {"size", 1}
    };
    nlohmann::json invalidTraits = {
        {"minDepth", -10},
        {"acceptedSoils", {{"sand", true}, {"clay", false}}},
        {"size", 1}
    };

    void SetUp() override {
        el::Configurations conf("debuglog.conf");
        el::Loggers::reconfigureAllLoggers(conf); 
    }
};

TEST_F(SoilRequirementsTest, InitializationWithValidTraits) {
    ASSERT_NO_THROW(SoilRequirements sr(validTraits));
}

TEST_F(SoilRequirementsTest, InitializationWithEmptyTraits) {
    EXPECT_ANY_THROW(SoilRequirements sr(emptyTraits));
}

TEST_F(SoilRequirementsTest, InitializationWithMissingTraits) {
    EXPECT_THROW(SoilRequirements sr(missingTraits), nlohmann::json::exception);
}

TEST_F(SoilRequirementsTest, InitializationWithExcessTraits) {
    ASSERT_NO_THROW(SoilRequirements sr(excessTraits));
}

TEST_F(SoilRequirementsTest, InitializationDepthFloat) {
    EXPECT_THROW(SoilRequirements sr(wrongTypeDepth), std::runtime_error);
}

TEST_F(SoilRequirementsTest, InitializationWithWrongTypeSoil) {
    EXPECT_THROW(SoilRequirements sr(wrongTypeSoil), nlohmann::json::exception);
}

TEST_F(SoilRequirementsTest, InitializationWithEmptySoilTraits) {
    EXPECT_THROW(SoilRequirements sr(emptySoilTraits), nlohmann::json::exception);
}

TEST_F(SoilRequirementsTest, InitializationWithInvalidTraits) {
    EXPECT_THROW(SoilRequirements sr(invalidTraits), std::invalid_argument);
}