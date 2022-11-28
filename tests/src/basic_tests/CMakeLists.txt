cmake_minimum_required (VERSION 3.7.2)

find_package(GTest REQUIRED)

# -- UNIT TESTS VALUE --
add_executable(franklyboot-basic-tests
  tests.cpp
)

target_link_libraries(franklyboot-basic-tests
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE frankly-bootloader
)

add_test(
  NAME franklyboot-basic-tests
  COMMAND franklyboot-basic-tests
)