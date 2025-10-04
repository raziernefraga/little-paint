cmake_minimum_required(VERSION 3.5)

project(LittlePaint LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ✅ Ativar geração automática de moc
set(CMAKE_AUTOMOC ON)

# Encontrar Qt
find_package(Qt5 COMPONENTS Widgets REQUIRED)

# Fontes do projeto
set(SOURCES
    main.cpp
    tool.cpp
    canvaswidget.cpp
    mainwindow.cpp
)

set(HEADERS
    tool.h
    canvaswidget.h
    mainwindow.h
)

# Criar executável
add_executable(LittlePaint
    ${SOURCES}
    ${HEADERS}
)

# Linkar com Qt
target_link_libraries(LittlePaint
    Qt5::Widgets
)

# Instalar (opcional)
install(TARGETS LittlePaint DESTINATION bin)
