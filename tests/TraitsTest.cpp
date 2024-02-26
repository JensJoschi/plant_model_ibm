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

// This file includes unit tests for the class Traits. See tests/CMake file 
// and google test documentation.

#include "Traits.h"

/** @cond */
#include "gtest/gtest.h"
#include "nlohmann/json.hpp"
#include "easylogging++.h"
/** @endcond */

class TraitsTest : public ::testing::Test {
    protected:
    nlohmann::json soilReqValid = {
        {"minDepth", 10},
        {"acceptedSoils", {{"sand", true}, {"clay", false}}}
    };
    nlohmann::json soilReqBroken = {
        {"acceptedSoils", {{"sand", true}, {"clay", false}}}
    };
    nlohmann::json soilReqInvalid = {
        {"minDepth", -10},
        {"acceptedSoils", {{"sand", true}, {"clay", false}}}
    };
    nlohmann::json resAllocValid = {
        {"conversionEfficiency", 0.5},
        {"maintenanceCosts", 0.1},
        {"seedAllocation", 0.2},
        {"biomassAllocation", 0.7},
        {"maxInvestment", 0.8},
        {"shadeFactor", 0.5}
    };
    nlohmann::json resAllocBroken = {
        {"conversionEfficiency", 0.5},
        {"seedAllocation", 0.2},
        {"biomassAllocation", 0.7},
        {"maxInvestment", 0.8}
    };
    nlohmann::json resAllocInvalid = {
        {"conversionEfficiency", -0.5},
        {"maintenanceCosts", 0.1},
        {"seedAllocation", 0.2},
        {"biomassAllocation", 0.7},
        {"maxInvestment", 0.8},
        {"shadeFactor", 0.5}
    };
    nlohmann::json seedBiologyValid = {
        {"Dormancy", false},
        {"GerminationSuccess", 0.1},
        {"MortalityDormant", 0.05},
        {"DormancyBreakRate", 0.8}
    };
    nlohmann::json seedBiologyBroken = {
        {"Dormancy", false},
        {"GerminationSuccess", 0.1},
        {"MortalityDormant", 0.05}
    };
    nlohmann::json seedBiologyInvalid = {
        {"Dormancy", false},
        {"GerminationSuccess", 0.05},
        {"MortalityDormant", 0.1},
        {"DormancyBreakRate", 0.8}
    };
    nlohmann::json lifeHistValid = {
        {"MaturationTime", 5},
        {"LifeSpan", 10},
        {"MaxHeight", 100},
    };
    nlohmann::json lifeHistBroken = {
        {"MaturationTime", 5},
        {"LifeSpan", 10},
    };
    nlohmann::json Int_Float_error = {
        {"MaturationTime", 5.99}, //float instead of int; conversion is valid (cast to int possible) but wrong (5 instead of 6)
        {"LifeSpan", 10},
        {"MaxHeight", 100},
    };
    nlohmann::json lifeHistInvalid = {
        {"MaturationTime", 5},
        {"LifeSpan", 10},
        {"MaxHeight", -100},
    };

    void SetUp() override {
        el::Configurations conf("debuglog.conf");
        el::Loggers::reconfigureAllLoggers(conf); 
    }
};

TEST_F(TraitsTest, allValid){
    EXPECT_NO_THROW(Traits(lifeHistValid, resAllocValid, soilReqValid, seedBiologyValid));
}

TEST_F(TraitsTest, orderWrong){
    EXPECT_THROW(Traits(seedBiologyValid, lifeHistValid, resAllocValid, soilReqValid), nlohmann::json::exception);
}

TEST_F(TraitsTest, JsonBroken){
    EXPECT_THROW(Traits(lifeHistBroken, resAllocValid, soilReqValid, seedBiologyValid), nlohmann::json::exception);
    EXPECT_THROW(Traits(lifeHistValid, resAllocBroken, soilReqValid, seedBiologyValid), nlohmann::json::exception);
    EXPECT_THROW(Traits(lifeHistValid, resAllocValid, soilReqBroken, seedBiologyValid), nlohmann::json::exception);
    EXPECT_THROW(Traits(lifeHistValid, resAllocValid, soilReqValid, seedBiologyBroken), nlohmann::json::exception);

    EXPECT_THROW(Traits(Int_Float_error, resAllocValid, soilReqValid, seedBiologyValid), std::runtime_error);
}

TEST_F(TraitsTest, TraitsInvalid){
    EXPECT_THROW(Traits(lifeHistInvalid, resAllocValid, soilReqValid, seedBiologyValid), std::invalid_argument);
    EXPECT_THROW(Traits(lifeHistValid, resAllocInvalid, soilReqValid, seedBiologyValid), std::invalid_argument);
    EXPECT_THROW(Traits(lifeHistValid, resAllocValid, soilReqInvalid, seedBiologyValid), std::invalid_argument);
    EXPECT_THROW(Traits(lifeHistValid, resAllocValid, soilReqValid, seedBiologyInvalid), std::invalid_argument);
}

