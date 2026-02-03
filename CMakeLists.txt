cmake_minimum_required(VERSION 3.16)

project(Sim8086 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/cpu8086.cpp
    src/cpu_exec.cpp
    src/cpu_ops.cpp
    src/managers/gui_manager.cpp
    src/managers/execution_manager.cpp
    src/managers/hardware_controller.cpp
    src/components/code_editor.cpp
    src/components/dos_terminal.cpp
    src/components/traffic_light.cpp
    src/components/register_view.cpp
    src/components/memory_view.cpp
    src/components/syntax_highlighter.cpp
    src/tools/converter.cpp
    src/tools/calculator.cpp
    src/tools/settings.cpp
)

# FIXED: Added MACOSX_BUNDLE (for Mac) and WIN32 (for Windows)
add_executable(Sim8086 MACOSX_BUNDLE WIN32 ${SOURCES})

target_link_libraries(Sim8086 PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)
