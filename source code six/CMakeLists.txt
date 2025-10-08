cmake_minimum_required(VERSION 3.10)
project(LittlePaint)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ativa o Meta-Object Compiler automático
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Encontra Qt5
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)

# Arquivos fonte
set(SOURCES
    main.cpp
    mainwindow.cpp
    canvaswidget.cpp
    tool.cpp
    undostack.cpp
)

set(HEADERS
    mainwindow.h
    canvaswidget.h
    tool.h
    UndoStack.h
)

# Cria executável
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Linka com Qt5
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets)

# Instala binário para AppImage
install(TARGETS ${PROJECT_NAME} DESTINATION usr/bin)

# Instala arquivo .desktop (sem ícone)
install(FILES LittlePaint.desktop DESTINATION usr/share/applications)
