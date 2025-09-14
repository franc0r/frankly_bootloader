cmake_minimum_required (VERSION 3.7.2)

find_package(GTest REQUIRED)

# -- UNIT TESTS VALUE --
add_executable(franklyboot-app-info-tests
  tests.cpp
)


target_include_directories(franklyboot-app-info-tests
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../frankly_test_utils/include/>
)


target_link_libraries(franklyboot-app-info-tests
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE frankly-bootloader
  PRIVATE franklyboot-test-utils
)

add_test(
  NAME franklyboot-app-info-tests
  COMMAND franklyboot-app-info-tests
)