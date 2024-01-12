# Copyright (C) 2023 - present Jens Joschinski
# This file is part of the ECOLOPES PLANT MODEL.

# ECOLOPES PLANT MODEL is free software: you can redistribute it and/or modify 
# it under the terms of the GNU General Public License as published by the 
# Free Software Foundation, either version 3 of the License, or (at your option) any later version.

# ECOLOPES PLANT MODEL is distributed in the hope that it will be useful, 
# but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
# or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

# You should have received a copy of the GNU General Public License along with ECOLOPES PLANT MODEL. 
# If not, see <https://www.gnu.org/licenses/>. */

# ECOLOPES PLANT MODEL is derived from FATE,https://github.com/leca-dev/RFate.git/ : 
#	Copyright (C) 2021 Isabelle Boulangeat, Damien Georges, Maya Guéguen,
#	Wilfried Thuiller

#Authors: Jens Joschinski

cmake_minimum_required(VERSION 3.0)
project(plants VERSION 1.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(WorkDir ${CMAKE_CURRENT_SOURCE_DIR})
add_definitions(-DWorkDir="${WorkDir}")

#create variable SRC with all files from folder src:
set (SRC "src/a_generalFunctions_vmvc.cpp"  "src/g_Data_BASE.cpp"   "src/g_Data_PLANTS.cpp" 
         "src/g_FGUtils.cpp"                "src/g_GSP_BASE.cpp"    "src/g_GSP_PLANTS.cpp" 
         "src/p_Cell.cpp"                   "src/p_FuncGroup.cpp"   "src/p_Individual.cpp" 
         "src/p_PFG.cpp"                    "src/p_PFGDefs.cpp"     "src/p_PFGdisturbance.cpp"
        "src/p_PlantModel.cpp"              "src/p_PlantResource.cpp" "src/p_PropPool.cpp" 
        "src/p_ResourceAlloc.cpp" "src/rng.cpp")

#library with all files from folder src:
add_library(Plants SHARED ${SRC} src/plants.cpp include/easylogging++.cc)

add_executable(Tests ${SRC} tests.cpp include/easylogging++.cc)

target_include_directories(Plants PUBLIC
                             "${PROJECT_SOURCE_DIR}/include"
                             "${PROJECT_SOURCE_DIR}/src"
                            )

target_include_directories(Tests PUBLIC
                            "${PROJECT_SOURCE_DIR}/include"
                            "${PROJECT_SOURCE_DIR}/src"
                           )