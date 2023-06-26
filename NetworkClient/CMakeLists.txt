cmake_minimum_required(VERSION 3.20)

include(../CMakeTools/CMakeLists.txt)
include(../Engine/CMakeTools/CMakeLists.txt)

project(NetworkClient)

set(CMAKE_CXX_STANDARD 17)

include_engine_dll()

file(GLOB_RECURSE SRC
        cpp/*.cpp
        include/*.h
        )

include_directories(
        include
)

include_engine(..)

add_executable(${PROJECT_NAME} ${SRC})
include_engine_pch(../..)

link_engine(${PROJECT_NAME} ..)