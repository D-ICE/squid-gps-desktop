project(squid-gps-desktop C CXX)
cmake_minimum_required(VERSION 3.11)

set(squid_gps_desktop_is_toplevel_project FALSE)
if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(squid_gps_desktop_is_toplevel_project TRUE)
endif()

# Avoid showing external dependencies at top level on Xcode
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 17)
option(squid_gps_desktop_enable_tests "Enable tests" ${squid_gps_desktop_is_toplevel_project})
option(squid_gps_enable_gui "Enable tests" ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/add_asio.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/add_marnav.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/add_json.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/add_fmt.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/add_spdlog.cmake)

add_subdirectory(src)
include_directories(src)
if(${squid_gps_desktop_enable_tests})
  enable_testing()
  include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/add_googletest.cmake)
  add_subdirectory(test)
endif()
