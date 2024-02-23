#Copyright (C)  2022 - present  Studio Animal-Aided Design

# This file is part of the INDIVIDUAL-BASED PLANT MODEL.

# INDIVIDUAL-BASED PLANT MODEL is free software: you can redistribute it and/or modify 
# it under the terms of the GNU General Public License as published by the 
# Free Software Foundation, either version 3 of the License, or (at your option) any later version.

# INDIVIDUAL-BASED PLANT MODEL is distributed in the hope that it will be useful, 
# but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
# or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

# You should have received a copy of the GNU General Public License along with INDIVIDUAL-BASED PLANT MODEL. 
# If not, see <https://www.gnu.org/licenses/>. */

# INDIVIDUAL-BASED PLANT MODEL is derived from the ECOLOPES PLANT MODEL, Copyright (C) 2022-present Studio Animal-Aided Design.

# ECOLOPES PLANT MODEL is derived from FATE,https://github.com/leca-dev/RFate.git/ : 
#	Copyright (C) 2021 Isabelle Boulangeat, Damien Georges, Maya Guéguen,
#	Wilfried Thuiller

#Authors: Jens Joschinski (EPM)

cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#get google test
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        f6f920d2ba09f3951af59045cd31c42e95bf127a 
  OVERRIDE_FIND_PACKAGE
)
FetchContent_MakeAvailable(googletest)

# list all tests here
set(TEST_SOURCES  SoilReqsTest.cpp
                  SeedBiologyTest.cpp
                  LifeHistoryTest.cpp
                  ResourceAllocTest.cpp
                  TraitsTest.cpp
                  ResourceTest.cpp
                  IndividualTest.cpp
                  SeedPoolTest.cpp

                  #to update:
                  DataTest.cpp
                  CellTest.cpp
                  PlantModelTest.cpp
                  PlantModelTestNoDisp.cpp

                  #to remove:
                 PropPoolTest.cpp
                 FuncGroupTest.cpp
                )

enable_testing()

add_executable("Tests_${SUFFIX}"  ${TEST_SOURCES})


target_link_libraries(Tests_${SUFFIX}  gtest_main "Plants_${SUFFIX}")
target_include_directories(Tests_${SUFFIX}  PUBLIC
                             "${CMAKE_SOURCE_DIR}/include"
                             "${CMAKE_SOURCE_DIR}/src"
)

include(GoogleTest)
gtest_discover_tests(Tests_${SUFFIX} )
