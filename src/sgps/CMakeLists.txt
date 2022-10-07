set(sgps_sources
  nmea_listener.h
  nmea_listener.cpp
)

add_library(sgps ${sgps_sources})
target_link_libraries(sgps asio marnav spdlog)

target_include_directories(sgps PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:include>
)

source_group(TREE "${CMAKE_CURRENT_LIST_DIR}" FILES ${sgps_sources})
set_target_properties(sgps PROPERTIES FOLDER src)
