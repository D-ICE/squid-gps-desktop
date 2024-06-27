find_package(Qt6 REQUIRED COMPONENTS Qml Quick Gui Widgets Core SerialPort)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(squid-gps-desktop
    main.cpp
    ${resources}
)

qt_add_qml_module(squid-gps-desktop
    URI "Backend"
    VERSION 1.0
    SOURCES
      backend.cpp
      backend.h
      usb.cpp
      usb.h
    QML_FILES
        Main.qml
    RESOURCES
      fonts/refresh.svg
      fonts/settings.svg
)

target_link_libraries(squid-gps-desktop
  PRIVATE
    Qt6::Qml
    Qt6::Quick
    Qt6::Widgets
    Qt6::Core
    Qt6::SerialPort
    sgps
)

set(squid-gps-desktop_libraries
  Qt6::Qml
  Qt6::Quick
  Qt6::Widgets
  Qt6::Core
  Qt6::SerialPort
  sgps
)
if(WIN32)
  set(squid-gps-desktop_libraries ${squid-gps-desktop_libraries} wsock32 ws2_32)
  target_compile_definitions(squid-gps-desktop PRIVATE CO_INITIALIZE)
endif()
target_link_libraries(squid-gps-desktop PRIVATE ${squid-gps-desktop_libraries})

set_target_properties(squid-gps-desktop PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)
