option(ENABLE_FTXUI "Use the FTXUI as interface for the console.")
cmake_minimum_required(VERSION 3.28.3)

project(machine
    LANGUAGES CXX
    VERSION 0.2
    DESCRIPTION "Simulation tools for classical models of computation."
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
    GIT_TAG v6.1.9
)

FetchContent_MakeAvailable(ftxui)

add_executable(run src/run.cpp)

add_executable(button src/button_in_frame.cpp)

target_link_libraries(button
    PRIVATE ftxui::screen
    PRIVATE ftxui::dom
    PRIVATE ftxui::component
)

add_library(machine STATIC
    src/string.cpp
    src/encoder.cpp
    src/number.cpp
    src/machine.cpp
    src/input.cpp
    src/output.cpp
    src/control.cpp
    src/stack.cpp
    src/queue.cpp
    src/unsigned_counter.cpp
    src/counter.cpp
    src/tape.cpp
)

add_library(console OBJECT src/console.cpp)

target_link_libraries(console
    PRIVATE ftxui::screen
    PRIVATE ftxui::dom
    PRIVATE ftxui::component
)

target_link_libraries(run PUBLIC machine)

add_executable(tui src/tui.cpp)

target_link_libraries(tui
    PRIVATE ftxui::screen
    PRIVATE ftxui::dom
    PRIVATE ftxui::component
    PUBLIC machine
)

target_link_libraries(tui PUBLIC console)

