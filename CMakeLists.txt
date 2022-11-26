cmake_minimum_required (VERSION 3.7.2)

project(frankly-bootloader)

add_compile_options(-Wall)# -O2)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(${PROJECT_NAME} SHARED
  src/francor/franklyboot/msg.cpp
)

target_include_directories(${PROJECT_NAME}
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
         $<INSTALL_INTERFACE:include/>
)

enable_testing()

add_subdirectory(tests)