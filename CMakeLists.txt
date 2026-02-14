cmake_minimum_required(VERSION 3.16)
project(SchachbrettEditor VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 QUIET COMPONENTS Svg)
qt_standard_project_setup()

qt_add_executable(${PROJECT_NAME}
    src/main.cpp
    src/MainWindow.cpp
    src/ChessBoardWidget.cpp
    src/ChessPosition.cpp
    src/PgnParser.cpp
    pieces.qrc
)

target_include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)

if(Qt6Svg_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Svg)
    target_compile_definitions(${PROJECT_NAME} PRIVATE HAS_QT_SVG)
    message(STATUS "Qt6 SVG gefunden - SVG-Figurenset aktiviert")
else()
    message(STATUS "Qt6 SVG nicht gefunden - Unicode-Fallback wird verwendet")
endif()
