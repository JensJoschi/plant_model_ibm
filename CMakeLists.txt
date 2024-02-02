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

cmake_minimum_required(VERSION 3.20)
project(plants VERSION 0.5.4)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(WorkDir ${CMAKE_CURRENT_SOURCE_DIR})
add_definitions(-DDEFAULT_WD="${WorkDir}/tests/")
configure_file(config.h.in ${PROJECT_SOURCE_DIR}/config.h)
configure_file(Doxyfile.in ${PROJECT_SOURCE_DIR}/Doxyfile)
configure_file(README.md.in ${PROJECT_SOURCE_DIR}/README.md)
configure_file(USER_GUIDE.md.in ${PROJECT_SOURCE_DIR}/USER_GUIDE.md)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/out)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/out)
set(SUFFIX "${CMAKE_SYSTEM_NAME}_${PROJECT_VERSION}")

add_subdirectory(src)
add_subdirectory(tests)