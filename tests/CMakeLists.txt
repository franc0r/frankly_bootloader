cmake_minimum_required (VERSION 3.7.2)

find_package(GTest REQUIRED)

# -- UNIT TEST LIBRARY --
add_subdirectory(frankly_test_utils)

# -- UNIT TESTS --
add_subdirectory(src/basic_tests)
add_subdirectory(src/general_request_tests)
add_subdirectory(src/device_infos)
add_subdirectory(src/flash_infos)
add_subdirectory(src/app_infos)
add_subdirectory(src/page_buffer)
add_subdirectory(src/flash_read)
add_subdirectory(src/flash_write)




