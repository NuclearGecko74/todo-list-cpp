cmake_minimum_required(VERSION 3.10)

project(TodoList)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(Dependencies.cmake)

add_subdirectory(Core)
add_subdirectory(App)