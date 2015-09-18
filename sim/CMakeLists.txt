cmake_minimum_required (VERSION 2.6)

project(sim)

include_directories(${SIM_INCLUDES})

add_library(sim SHARED
  elevator.cpp
  logging.cpp
  scheduler.cpp
  types.cpp
)
