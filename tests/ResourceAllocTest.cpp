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

// This file includes unit tests for the class ResourceAlloc (part of the traits definitions). See tests/CMake file 
// and google test documentation.

#include "ResourceAlloc.h"

/** @cond */
#include "gtest/gtest.h"
#include "nlohmann/json.hpp"
#include "easylogging++.h"
/** @endcond */


class ResAllocTest : public ::testing::Test {
protected:
    nlohmann::json validTraits = {
        {"conversionEfficiency", 0.5},
        {"maintenanceCosts", 0.1},
        {"seedAllocation", 0.2},
        {"biomassAllocation", 0.7},
        {"maxBiomass", 1000}
    };
    nlohmann::json emptyTraits = {};
    nlohmann::json missingTraits = {
        {"conversionEfficiency", 0.5},
        {"maintenanceCosts", 0.1},
        {"biomassAllocation", 0.7},
        {"maxBiomass", 1000},
    };
    nlohmann::json excessTraits = {
        {"conversionEfficiency", 0.5},
        {"maintenanceCosts", 0.1},
        {"seedAllocation", 0.2},
        {"biomassAllocation", 0.7},
        {"maxBiomass", 1000},
        {"excess", 4.2123}
    };
    nlohmann::json wrongTypemaxInvest = {
        {"conversionEfficiency", 0.5},
        {"maintenanceCosts", 0.1},
        {"seedAllocation", 0.2},
        {"biomassAllocation", 0.7},
        {"maxBiomass", "1000"}
    };
    nlohmann::json ConvEffInt = {
        {"conversionEfficiency", 1},
        {"maintenanceCosts", 0.1},
        {"seedAllocation", 0.2},
        {"biomassAllocation", 0.7},
        {"maxBiomass", 1000}
    };
    nlohmann::json ConvEffInt2 = {
        {"conversionEfficiency", 2},
        {"maintenanceCosts", 0.1},
        {"seedAllocation", 0.2},
        {"biomassAllocation", 0.7},
        {"maxBiomass", 1000}
    };
    nlohmann::json ConvEffNeg = {
        {"conversionEfficiency", -0.5},
        {"maintenanceCosts", 0.1},
        {"seedAllocation", 0.2},
        {"biomassAllocation", 0.7},
        {"maxBiomass", 1000}
    };
    nlohmann::json maintCostNeg = {
        {"conversionEfficiency", 0.5},
        {"maintenanceCosts", -0.1},
        {"seedAllocation", 0.2},
        {"biomassAllocation", 0.7},
        {"maxBiomass", 1000}
    };
    nlohmann::json seedAllocNeg = {
        {"conversionEfficiency", 0.5},
        {"maintenanceCosts", 0.1},
        {"seedAllocation", -0.2},
        {"biomassAllocation", 0.7},
        {"maxBiomass", 1000}
    };
    nlohmann::json biomassAllocNeg = {
        {"conversionEfficiency", 0.5},
        {"maintenanceCosts", 0.1},
        {"seedAllocation", 0.2},
        {"biomassAllocation", -0.7},
        {"maxBiomass", 1000}
    };
    nlohmann::json maxBiomassNeg = {
        {"conversionEfficiency", 0.5},
        {"maintenanceCosts", 0.1},
        {"seedAllocation", 0.2},
        {"biomassAllocation", 0.7},
        {"maxBiomass", -1000}
    };
    nlohmann::json convEffLarge = {
        {"conversionEfficiency", 1.5},
        {"maintenanceCosts", 0.1},
        {"seedAllocation", 0.2},
        {"biomassAllocation", 0.7},
        {"maxBiomass", 1000}
    };
    nlohmann::json maintCLarge = {
        {"conversionEfficiency", 0.5},
        {"maintenanceCosts", 1.1},
        {"seedAllocation", 0.2},
        {"biomassAllocation", 0.7},
        {"maxBiomass", 1000}
    };
    nlohmann::json seedAllocLarge = {
        {"conversionEfficiency", 0.5},
        {"maintenanceCosts", 0.1},
        {"seedAllocation", 1.2},
        {"biomassAllocation", 0.7},
        {"maxBiomass", 1000}
    };
    nlohmann::json biomassAllocLarge = {
        {"conversionEfficiency", 0.5},
        {"maintenanceCosts", 0.1},
        {"seedAllocation", 0.2},
        {"biomassAllocation", 15.7},
        {"maxBiomass", 1000}
    };
    nlohmann::json allocSum = {
        {"conversionEfficiency", 0.5},
        {"maintenanceCosts", 0.1},
        {"seedAllocation", 0.2},
        {"biomassAllocation", 0.9},
        {"maxBiomass", 1000}
    };

    void SetUp() override {
        el::Configurations conf("debuglog.conf");
        el::Loggers::reconfigureAllLoggers(conf); 
    }
};

TEST_F(ResAllocTest, ValidTraits) {
    ASSERT_NO_THROW(ResourceAlloc r(validTraits));
}
TEST_F(ResAllocTest, EmptyTraits) {
    EXPECT_ANY_THROW(ResourceAlloc r(emptyTraits));
}
TEST_F(ResAllocTest, MissingTraits) {
    ASSERT_THROW(ResourceAlloc r(missingTraits), nlohmann::json::exception);
}
TEST_F(ResAllocTest, ExcessTraits) {
    ASSERT_NO_THROW(ResourceAlloc r(excessTraits));
}
TEST_F(ResAllocTest, WrongType) {
    ASSERT_THROW(ResourceAlloc r(wrongTypemaxInvest), nlohmann::json::exception);
}
TEST_F(ResAllocTest, ConvEffInt) {
    ASSERT_NO_THROW(ResourceAlloc r(ConvEffInt)); //vals of 0 or 1 are valid
    ASSERT_THROW(ResourceAlloc r(ConvEffInt2), std::invalid_argument);
}
TEST_F(ResAllocTest, ValsOutOfRange) {
    ASSERT_THROW(ResourceAlloc r(ConvEffNeg), std::invalid_argument);
    ASSERT_THROW(ResourceAlloc r(maintCostNeg), std::invalid_argument);
    ASSERT_THROW(ResourceAlloc r(seedAllocNeg), std::invalid_argument);
    ASSERT_THROW(ResourceAlloc r(biomassAllocNeg), std::invalid_argument);
    ASSERT_THROW(ResourceAlloc r(maxBiomassNeg), std::invalid_argument);

    ASSERT_THROW(ResourceAlloc r(convEffLarge), std::invalid_argument);
    ASSERT_THROW(ResourceAlloc r(maintCLarge), std::invalid_argument);
    ASSERT_THROW(ResourceAlloc r(seedAllocLarge), std::invalid_argument);
    ASSERT_THROW(ResourceAlloc r(biomassAllocLarge), std::invalid_argument);
}
TEST_F(ResAllocTest, SumAlloc) {
    ASSERT_THROW(ResourceAlloc r(allocSum), std::invalid_argument);
}