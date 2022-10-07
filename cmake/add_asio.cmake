include(FetchContent)

FetchContent_Declare(asio
  GIT_REPOSITORY https://github.com/chriskohlhoff/asio
  GIT_TAG asio-1-24-0
)

FetchContent_GetProperties(asio)
if(NOT asio_POPULATED)
  FetchContent_Populate(asio)
  # see https://think-async.com/Asio/AsioStandalone.html
  add_library(asio INTERFACE)
  target_include_directories(asio INTERFACE
    $<BUILD_INTERFACE:${asio_SOURCE_DIR}/asio/include>
    $<INSTALL_INTERFACE:include>
  )
  target_compile_definitions(asio INTERFACE ASIO_STANDALONE)
endif()
