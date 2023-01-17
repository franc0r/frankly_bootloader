cmake_minimum_required (VERSION 3.7.2)

# -- UNIT TESTS VALUE --
add_library(franklyboot-device-sim-api
  src/device_sim_api.cpp
)


target_include_directories(franklyboot-device-sim-api
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)


target_link_libraries(franklyboot-device-sim-api
  PRIVATE frankly-bootloader
)
