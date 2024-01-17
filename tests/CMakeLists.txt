cmake_minimum_required(VERSION 3.10)

project(Tests VERSION 0.5.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_SOURCE_DIR}/src  # contains the private header files
                    ${CMAKE_SOURCE_DIR}/include) 

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        f6f920d2ba09f3951af59045cd31c42e95bf127a 
  OVERRIDE_FIND_PACKAGE
)
FetchContent_MakeAvailable(googletest)

# list all tests here
set(TEST_SOURCES ${CMAKE_SOURCE_DIR}/tests/PropPoolTest.cpp)

# list all according files with the classes here
set (SRC "${CMAKE_SOURCE_DIR}/src/p_PropPool.cpp" 
)
enable_testing()

set(NAME "Tests_${CMAKE_SYSTEM_NAME}_${PROJECT_VERSION}")
add_executable(${NAME}  ${TEST_SOURCES} ${SRC})


target_link_libraries(${NAME}  gtest_main)
target_include_directories(${NAME}  PUBLIC
                             "${PROJECT_SOURCE_DIR}/include"
                             "${PROJECT_SOURCE_DIR}/src"
)

include(GoogleTest)
gtest_discover_tests(${NAME} )
