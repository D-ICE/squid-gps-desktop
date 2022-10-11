include(FetchContent)

FetchContent_Declare(marnav
  URL https://github.com/mariokonrad/marnav/archive/refs/tags/v0.13.0.zip
  PATCH_COMMAND git init && git apply --verbose --ignore-whitespace "${PROJECT_SOURCE_DIR}/cmake/patches/marnav.patch"
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
