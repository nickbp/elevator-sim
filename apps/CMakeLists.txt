cmake_minimum_required (VERSION 2.6)

project(apps)

include_directories(${SIM_INCLUDES})

add_executable(sim-sample sim-sample.cpp)
target_link_libraries(sim-sample sim)
