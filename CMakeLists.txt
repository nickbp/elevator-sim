cmake_minimum_required (VERSION 2.6)

# GTest options

find_path(gtest_INCLUDE_DIR NAMES gtest/gtest.h)
find_library(gtest_LIBRARY NAMES gtest)

if(gtest_INCLUDE_DIR AND gtest_LIBRARY)
  message(STATUS "Found gtest")
  set(FOUND_GTEST ON)
else()
  message(STATUS "Didn't find gtest")
endif()

option(BUILD_TESTS "Build unit tests" ${FOUND_GTEST})

# Enable C++11 and more warnings
if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "-std=c++0x -Wall")
endif()

set(SIM_INCLUDES ${PROJECT_SOURCE_DIR})

add_subdirectory(apps)
add_subdirectory(sim)
if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
