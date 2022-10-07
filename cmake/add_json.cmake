include(FetchContent)

FetchContent_Declare(json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.9.1
)
FetchContent_GetProperties(json)
if(NOT json_POPULATED)
  set(JSON_BuildTests OFF CACHE BOOL "")
  set(JSON_Install ON CACHE BOOL "")
  set(JSON_MultipleHeaders OFF CACHE BOOL "")
  FetchContent_Populate(json)
  add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR})
endif()
