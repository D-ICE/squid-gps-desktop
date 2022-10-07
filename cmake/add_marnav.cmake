include(FetchContent)

FetchContent_Declare(marnav
  GIT_REPOSITORY https://github.com/mariokonrad/marnav.git
  GIT_TAG v0.13.0
)

FetchContent_GetProperties(marnav)
if(NOT marnav_POPULATED)
  FetchContent_Populate(marnav)

  set(ENABLE_EXAMPLES OFF CACHE BOOL "" FORCE)
  set(ENABLE_TESTS OFF CACHE BOOL "" FORCE)
  set(ENABLE_TESTS_BENCHMARK OFF CACHE BOOL "" FORCE)
  set(ENABLE_TOOLS OFF CACHE BOOL "" FORCE)
  
  add_subdirectory(${marnav_SOURCE_DIR} ${marnav_BINARY_DIR})
endif()

set_target_properties(marnav PROPERTIES FOLDER deps)
