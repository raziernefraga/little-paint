cmake_minimum_required(VERSION 3.5)

project(LitePaint LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)  # <- Isso ativa o Meta-Object Compiler automaticamente

find_package(Qt5 REQUIRED COMPONENTS Widgets Gui)

qt5_add_resources(RESOURCE_SOURCES resources/resources.qrc)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(LitePaint
    main.cpp
    mainwindow.cpp
    canvaswidget.cpp
    tool.cpp
    undostack.cpp
    ${RESOURCE_SOURCES}
)

target_link_libraries(LitePaint PRIVATE Qt5::Widgets Qt5::Gui)
