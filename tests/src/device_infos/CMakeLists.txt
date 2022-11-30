cmake_minimum_required (VERSION 3.7.2)

find_package(GTest REQUIRED)

# -- UNIT TESTS VALUE --
add_executable(franklyboot-device-info-tests
  tests.cpp
  ../test_utils.cpp
)


target_include_directories(franklyboot-device-info-tests
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include/>
)


target_link_libraries(franklyboot-device-info-tests
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE frankly-bootloader
)

add_test(
  NAME franklyboot-device-info-tests
  COMMAND franklyboot-device-info-tests
)