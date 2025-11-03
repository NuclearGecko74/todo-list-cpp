cmake_minimum_required(VERSION 3.20)

project(TodoList LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(raylib CONFIG REQUIRED)

add_executable(TodoList
        src/main.cpp
        src/Task.cpp
        src/App.cpp
)

target_include_directories(TodoList PUBLIC include)

target_link_libraries(TodoList PRIVATE
        raylib
)