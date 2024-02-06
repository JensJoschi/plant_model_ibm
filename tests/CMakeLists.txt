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
set(TEST_SOURCES PropPoolTest.cpp
                 FuncGroupTest.cpp
                 ResourceTest.cpp
                 CellTest.cpp
                 PlantModelTest.cpp
                 PlantModelTestNoDisp.cpp
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
