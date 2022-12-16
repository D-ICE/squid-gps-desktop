add_executable(squid-gps-cli
  main.cpp
)

target_link_libraries(squid-gps-cli
  sgps
  cxxopts
  spdlog
)
