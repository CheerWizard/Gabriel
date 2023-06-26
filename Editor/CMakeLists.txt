cmake_minimum_required(VERSION 3.20)

include(../CMakeTools/CMakeLists.txt)
include(../Engine/CMakeTools/CMakeLists.txt)

project(Editor)

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

replace_dirs(Engine/shaders Editor/shaders)
replace_dirs(Editor/Assets/images Editor/Assets/images)
replace_dirs(Editor/Assets/models Editor/Assets/models)
replace_dirs(Editor/Assets/fonts Editor/Assets/fonts)

configure_file(Gabriel_Logo.png Gabriel_Logo.png COPYONLY)

link_engine(${PROJECT_NAME} ..)

if(IMGUI)

    if(DEBUG)
        configure_file(../Engine/shaders/build_imgui_debug/core.glsl shaders/core.glsl COPYONLY)
    else()
        configure_file(../Engine/shaders/build_imgui_release/core.glsl shaders/core.glsl COPYONLY)
    endif(DEBUG)

else()

    if(DEBUG)
        configure_file(../Engine/shaders/build_debug/core.glsl shaders/core.glsl COPYONLY)
    else()
        configure_file(../Engine/shaders/build_release/core.glsl shaders/core.glsl COPYONLY)
    endif(DEBUG)

endif(IMGUI)