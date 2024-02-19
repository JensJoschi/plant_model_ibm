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


#include "Data_PLANTS.h"
#include "GSP_PLANTS.h"
#include "gtest/gtest.h"

// This file includes unit tests for the class Data_PLANTS, which is derived from jointmodel/Data_BASE. See tests/CMake file 
// and google test documentation.

class DataTest : public testing::Test {
    protected:
    Data_PLANTS* data;
    const GSP_PLANTS* standard_gsp;
        void SetUp() override {
            standard_gsp = new GSP_PLANTS("test.json");
    }

    void TearDown() override {
        delete data;
    }
};

// TEST_F(DataTest, FileDoesNotExist){
//     EXPECT_THROW(Data_PLANTS foo("non_existing_file", *standard_gsp), nlohmann::json_abi_v3_11_2::detail::parse_error);
    //SIGABORTS, does not throw and thus is very hard to test
// }

TEST_F(DataTest, LogConfig){
    data = new Data_PLANTS("dataTests/noLogConf.json", *standard_gsp);
    EXPECT_EQ(data->logger, "plantlog.conf"); //Default
    data = new Data_PLANTS("dataTests/LogConf.json", *standard_gsp);
    EXPECT_EQ(data->logger, "foo.conf");
}
TEST_F(DataTest, readOK){
    data = new Data_PLANTS("dataTests/test.json", *standard_gsp);
}
