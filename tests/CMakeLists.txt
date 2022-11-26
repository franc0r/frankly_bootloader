cmake_minimum_required (VERSION 3.7.2)

find_package(GTest REQUIRED)

# -- UNIT TESTS VALUE --
add_executable(unit-test-franklyboot
  src/test_franklyboot.cpp
)

target_link_libraries(unit-test-franklyboot
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE frankly-bootloader
)

add_test(
  NAME unit-test-franklyboot
  COMMAND unit-test-franklyboot
)