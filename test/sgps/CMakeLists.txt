set(sgps_test_sources
  sgps_test.cpp
)

add_executable(sgps_test ${sgps_test_sources})

target_link_libraries(sgps_test sgps gtest gtest_main spdlog)
target_compile_definitions(sgps_test PRIVATE FRAMES_TXT="${CMAKE_CURRENT_LIST_DIR}/frames.txt")

source_group(TREE "${CMAKE_CURRENT_LIST_DIR}" FILES ${sgps_test_sources})
set_target_properties(sgps_test PROPERTIES FOLDER test)
