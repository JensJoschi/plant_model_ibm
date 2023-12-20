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

project(plantlib VERSION 1.0)

#work directory
set(WorkDir ${CMAKE_CURRENT_SOURCE_DIR})
add_definitions(-DWorkDir="${WorkDir}")

include_directories(include)
add_library(PlantLib SHARED 
a_generalFunctions_vmvc.cpp     p_Cell.cpp                      p_PlantModel.cpp
g_Data_BASE.cpp                 p_FuncGroup.cpp                 p_PlantResource.cpp
g_Data_PLANTS.cpp               p_Individual.cpp                p_PropPool.cpp
g_FGUtils.cpp                   p_PFG.cpp                       p_ResourceAlloc.cpp
g_GSP_BASE.cpp                  p_PFGDefs.cpp                   rng.cpp
g_GSP_PLANTS.cpp                p_PFGdisturbance.cpp
/Users/jensjoschinski/plantmodel/include/easylogging++.cc)

add_executable(Tests 
a_generalFunctions_vmvc.cpp     p_Cell.cpp                      p_PlantModel.cpp
g_Data_BASE.cpp                 p_FuncGroup.cpp                 p_PlantResource.cpp
g_Data_PLANTS.cpp               p_Individual.cpp                p_PropPool.cpp
g_FGUtils.cpp                   p_PFG.cpp                       p_ResourceAlloc.cpp
g_GSP_BASE.cpp                  p_PFGDefs.cpp                   rng.cpp
g_GSP_PLANTS.cpp                p_PFGdisturbance.cpp
/Users/jensjoschinski/plantmodel/include/easylogging++.cc       tests.cpp)

target_include_directories(PlantLib PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           "${PROJECT_SOURCE_DIR}/src"
                           "${PROJECT_SOURCE_DIR}/include"
                           /Users/jensjoschinski/plantmodel/include
                           )