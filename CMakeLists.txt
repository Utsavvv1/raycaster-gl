# Copyright 2024 Betamark Pty Ltd. All rights reserved.
# Author: Shlomi Nissan (shlomi@betamark.com)

cmake_minimum_required(VERSION 3.22.1)

project(raycaster-gl)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message(${CMAKE_SOURCE_DIR}/cmake)

include(cmake/ShaderString.cmake)
ShaderString()

find_package(fmt REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

set(CORE_SOURCES
    src/core/mesh.cpp
    src/core/mesh.hpp
    src/core/pixels.cpp
    src/core/pixels.hpp
    src/core/plane.cpp
    src/core/plane.hpp
    src/core/shader.cpp
    src/core/shader.hpp
    src/core/timer.hpp
    src/core/window.cpp
    src/core/window.hpp
)

set(LIBS_SOURCES
    libs/glad/glad.c
)

set(GAME_SOURCES
    src/game/level.cpp
    src/game/level.hpp
    src/game/main.cpp
    src/game/player.cpp
    src/game/player.hpp
    src/game/shared.hpp
)

add_executable(raycaster-gl ${LIBS_SOURCES} ${CORE_SOURCES} ${GAME_SOURCES})

target_include_directories(raycaster-gl PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/libs
)

target_link_libraries(raycaster-gl PRIVATE fmt::fmt glfw OpenGL::GL)