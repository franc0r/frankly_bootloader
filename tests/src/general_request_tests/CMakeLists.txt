cmake_minimum_required (VERSION 3.7.2)

find_package(GTest REQUIRED)

# -- UNIT TESTS VALUE --
add_executable(franklyboot-general-request-tests
  tests.cpp
)

target_link_libraries(franklyboot-general-request-tests
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE frankly-bootloader
)

add_test(
  NAME franklyboot-general-request-tests
  COMMAND franklyboot-general-request-tests
)