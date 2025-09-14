cmake_minimum_required (VERSION 3.7.2)

find_package(GTest REQUIRED)

# -- UNIT TESTS VALUE --
add_library(franklyboot-test-utils
  src/frankly_test_utils.cpp
)

target_include_directories(franklyboot-test-utils
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
)

target_link_libraries(franklyboot-test-utils
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE frankly-bootloader
)
