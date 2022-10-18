set(sgps_sources
  model.h
  model.cpp
  controller.h
  controller.cpp

  nmea_listener.h
  nmea_listener.cpp
  squid_gps_server.h
  squid_gps_server.cpp
)

add_library(sgps ${sgps_sources})

set(sgps_libraries
  asio
  marnav
  nlohmann_json
  fmt
  spdlog
)
if(WIN32)
  set(sgps_libraries ${sgps_libraries} wsock32 ws2_32)
endif()
target_link_libraries(sgps ${sgps_libraries})

target_include_directories(sgps PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:include>
)

source_group(TREE "${CMAKE_CURRENT_LIST_DIR}" FILES ${sgps_sources})
set_target_properties(sgps PROPERTIES FOLDER src)
