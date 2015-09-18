cmake_minimum_required (VERSION 2.6)

project(tests)

include_directories(
  ${gtest_INCLUDE_DIR}
  ${SIM_INCLUDES}
)

find_package(Threads)
set(gtest_libs ${gtest_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})

# unit tests

add_executable(test-elevator test-elevator.cpp)
target_link_libraries(test-elevator sim ${gtest_libs})
add_test(test-elevator test-elevator)

add_executable(test-scheduler test-scheduler.cpp)
target_link_libraries(test-scheduler sim ${gtest_libs})
add_test(test-scheduler test-scheduler)
