find_package(Qt6 REQUIRED Qml Quick Gui Widgets)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

qt_standard_project_setup()

qt_add_resources(resources qml.qrc)

qt_add_executable(squid-gps-desktop
    main.cpp
    ${resources}
)

qt_add_qml_module(squid-gps-desktop
    URI backend
    VERSION 1.0
    SOURCES
      backend.cpp
      backend.h
)

target_link_libraries(squid-gps-desktop
  PRIVATE
    Qt6::Qml
    Qt6::Quick
    Qt6::Widgets
    sgps
)

set_target_properties(squid-gps-desktop PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)
