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

cmake_minimum_required(VERSION 3.0)

project(PlantModel, VERSION 1.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#If(APPLE)
#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")  # has currently no effect though
#endif()

#work directory
set(DEFAULT_WD ${CMAKE_CURRENT_SOURCE_DIR}) #if this approach doesnt work, try:
add_definitions(-DDEFAULT_WD="${DEFAULT_WD}")

include_directories(include)

add_subdirectory(src)

add_executable(main maintest.cpp /Users/jensjoschinski/plantmodel/include/easylogging++.cc)
# this is a main that includes plant model as library. 
target_link_libraries(main PUBLIC PlantLib)

target_include_directories(main PUBLIC
                           /Users/jensjoschinski/plantmodel/include/
                           )