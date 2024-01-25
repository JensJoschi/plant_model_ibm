cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set (SRC "g_Data_PLANTS.cpp"            "g_FGUtils.cpp"      "g_GSP_PLANTS.cpp" 
         "p_Cell.cpp"                   "p_FuncGroup.cpp"   "p_Individual.cpp" 
         "p_PFG.cpp"                    "p_PFGDefs.cpp"     "p_PFGdisturbance.cpp"
         "p_PlantModel.cpp"             "p_PlantResource.cpp" "p_PropPool.cpp" 
         "p_ResourceAlloc.cpp"          "rng.cpp")

set (JOINT "${CMAKE_SOURCE_DIR}/include/jointModel/a_generalFunctions_vmvc.cpp"  "${CMAKE_SOURCE_DIR}/include/jointModel/g_Data_BASE.cpp"    "${CMAKE_SOURCE_DIR}/include/jointModel/g_GSP_BASE.cpp")

add_library("Plants_${SUFFIX}" SHARED   ${SRC} 
                                        plants.cpp 
                                        ${CMAKE_SOURCE_DIR}/include/easylogging++.cc
                                        ${JOINT})

add_executable("Model_${SUFFIX}"    ${SRC} 
                                    main.cpp 
                                    ${CMAKE_SOURCE_DIR}/include/easylogging++.cc
                                    ${JOINT})

target_include_directories("Plants_${SUFFIX}" PUBLIC
                            "${CMAKE_SOURCE_DIR}/include"
                            "${CMAKE_SOURCE_DIR}/include/jointModel"
                            "${PROJECT_SOURCE_DIR}"
                           )

target_include_directories("Model_${SUFFIX}" PUBLIC
                           "${CMAKE_SOURCE_DIR}/include"
                            "${CMAKE_SOURCE_DIR}/include/jointModel"
                            "${PROJECT_SOURCE_DIR}"
                          )