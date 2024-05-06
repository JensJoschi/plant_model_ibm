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

cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
message(STATUS "compiling source code\n")

## ---- find sources ----

set (SRC "SeedPool.cpp"         "SeedBiology.cpp"    
         "PlantGrowth.cpp"      "LifeHistory.cpp"     
         "HabSuit.cpp"          "SoilRequirements.cpp" 
         "PlantResource.cpp"    "ResourceAlloc.cpp"
         "plantShape.cpp"
        "Individual.cpp"        "Traits.cpp"
        "Community.cpp"         "Illumination.cpp"
         "rng.cpp"
         "Data_PLANTS.cpp"  "GSP_PLANTS.cpp" 
         #to update
        "PlantModel.cpp"  
        "SeedDistribution.cpp"
        "PFGDefs.cpp"  "PFGdisturbance.cpp"
#to remove
        "PFG.cpp"             
        "FGUtils.cpp"
)
if(NOT SRC)
    message(FATAL_ERROR "SRC is empty")
endif()

#set also in parent scope so that tests can find them. Requires adding proper path:
set(DIR "${CMAKE_SOURCE_DIR}/src/")
foreach(SRC_FILE ${SRC})
    list(APPEND SRC_FILES "${DIR}${SRC_FILE}")
endforeach()
set (SRC_FILES ${SRC_FILES} PARENT_SCOPE)

set (JOINT  "${CMAKE_SOURCE_DIR}/include/jointModel/g_Data_BASE.cpp"    
            "${CMAKE_SOURCE_DIR}/include/jointModel/g_GSP_BASE.cpp"
            #these dependencies are not available as precompiled library yet
)

#debug info
message(STATUS "SRC_FILES:")
foreach(SRC_FILE ${SRC_FILES})
    message(STATUS "${SRC_FILE}")
endforeach()
message(STATUS "\nJOINT:")
foreach(JOINT_FILE ${JOINT})
    message(STATUS "${JOINT_FILE}")
endforeach()

# ---- create binaries ----

# add_library("Plants_${SUFFIX}" SHARED   ${SRC} 
#                                         plants.cpp 
#                                         ${CMAKE_SOURCE_DIR}/include/easylogging++.cc
#                                         ${JOINT})

add_executable("Model_${SUFFIX}"    ${SRC} 
                                    main.cpp 
                                    ${CMAKE_SOURCE_DIR}/include/easylogging++.cc
                                    ${JOINT})

# target_include_directories("Plants_${SUFFIX}" PUBLIC
#                             "${CMAKE_SOURCE_DIR}/include"
#                             "${CMAKE_SOURCE_DIR}/include/jointModel"
#                             "${PROJECT_SOURCE_DIR}"
#                            )

target_include_directories("Model_${SUFFIX}" PUBLIC
                           "${CMAKE_SOURCE_DIR}/include"
                            "${CMAKE_SOURCE_DIR}/include/jointModel"
                            "${PROJECT_SOURCE_DIR}"
                          )