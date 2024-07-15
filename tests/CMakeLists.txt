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
message(STATUS "\n\ncompiling tests\n")

# -- get google test ---
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        f6f920d2ba09f3951af59045cd31c42e95bf127a 
  OVERRIDE_FIND_PACKAGE
)
FetchContent_MakeAvailable(googletest)
find_package(Threads REQUIRED)
# --- find sources ---
set(TEST_SOURCES  SoilReqsTest.cpp        HabSuitTest.cpp
                  SeedBiologyTest.cpp     SeedPoolTest.cpp
                  LifeHistoryTest.cpp     PlantGrowthTest.cpp
                  ResourceAllocTest.cpp   ResourceTest.cpp
                  PlantShapeTest.cpp
                  TraitsTest.cpp          IndividualTest.cpp
                  CommunityTest.cpp
                  IlluminationTest.cpp
                  #VoxelTest.cpp
                 PlantModelTest           NeighbourTest.cpp
                 #SeedDistributionTest
                )

enable_testing()

set (JOINT  "${CMAKE_SOURCE_DIR}/include/jointModel/g_Data_BASE.cpp"    
            "${CMAKE_SOURCE_DIR}/include/jointModel/g_GSP_BASE.cpp"
            #these dependencies are not available as precompiled library yet
)

#easylogger requires initialization in some source file. Usually this would be done in main, but here we have no main.
#it is arbitrarily done in LifeHistoryTest.cpp.

if(NOT SRC_FILES)
    message(FATAL_ERROR "list of source files is empty")
endif()

#debug info
message(STATUS "tests:")
foreach(T ${TEST_SOURCES})
    message(STATUS "${T}")
endforeach()
# message(STATUS "source code:")
# foreach(F ${SRC_FILES})
#     message(STATUS "${F}")
# endforeach()
# message(STATUS "\nJOINT:")
# foreach(JOINT_FILE ${JOINT})
#     message(STATUS "${JOINT_FILE}")
# endforeach()

# ---- create binaries ----

add_executable(Tests_${SUFFIX}  ${TEST_SOURCES}    
                                ${SRC_FILES}
                                ${CMAKE_SOURCE_DIR}/include/easylogging++.cc
                                ${JOINT}
 )
 target_compile_definitions(Tests_${SUFFIX} PRIVATE TESTING=1)
target_link_libraries(Tests_${SUFFIX}  gtest_main Threads::Threads )#"Plants_${SUFFIX}")
target_include_directories(Tests_${SUFFIX}  PUBLIC
                             "${CMAKE_SOURCE_DIR}/include"
                             "${CMAKE_SOURCE_DIR}/include/jointModel"
                             "${CMAKE_SOURCE_DIR}/src"
                             "${PROJECT_SOURCE_DIR}"
)

include(GoogleTest)
gtest_discover_tests(Tests_${SUFFIX} )
