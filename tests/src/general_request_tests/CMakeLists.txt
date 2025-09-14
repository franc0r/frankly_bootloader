cmake_minimum_required (VERSION 3.7.2)

find_package(GTest REQUIRED)

# -- UNIT TESTS VALUE --
add_executable(franklyboot-general-request-tests
  tests.cpp
)

target_include_directories(franklyboot-general-request-tests
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../frankly_test_utils/include/>
)


target_link_libraries(franklyboot-general-request-tests
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE frankly-bootloader
  PRIVATE franklyboot-test-utils
)

add_test(
  NAME franklyboot-general-request-tests
  COMMAND franklyboot-general-request-tests
)