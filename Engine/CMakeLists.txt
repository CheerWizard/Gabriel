cmake_minimum_required(VERSION 3.20)

include(../CMakeTools/CMakeLists.txt)
include(CMakeTools/CMakeLists.txt)

project(Engine)

set(CMAKE_CXX_STANDARD 17)

build_engine_dll()

file(GLOB_RECURSE SRC
        cpp/*.cpp
        include/*.h

        ../Vendor/glad-c/src/glad.c

        ../Vendor/stb/stb_image.h
        ../Vendor/stb/stb_image_write.h
        ../Vendor/stb/stb_image_resize.h

        ../Vendor/imgui/imgui.cpp
        ../Vendor/imgui/imgui_draw.cpp
        ../Vendor/imgui/imgui_tables.cpp
        ../Vendor/imgui/imgui_widgets.cpp
        ../Vendor/imgui/imgui_demo.cpp
        ../Vendor/imgui/backends/imgui_impl_glfw.cpp
        ../Vendor/imgui/backends/imgui_impl_opengl3.cpp
        ../Vendor/imgui/misc/cpp/imgui_stdlib.cpp

        ../Vendor/ImGuizmo/ImGuizmo.cpp
        ../Vendor/ImGuizmo/ImGradient.cpp
        ../Vendor/ImGuizmo/ImSequencer.cpp
        ../Vendor/ImGuizmo/ImCurveEdit.cpp
        ../Vendor/ImGuizmo/GraphEditor.cpp
)

include_directories(
        include
        ../Vendor/glfw/include
        ../Vendor/glad-c/include
        ../Vendor/glm/glm
        ../Vendor/stb
        ../Vendor/assimp/include
        ../Vendor/freetype/include
        ../Vendor/imgui
        ../Vendor/spdlog/include
        ../Vendor/ImGuizmo
)

add_library(${PROJECT_NAME} SHARED ${SRC})
target_precompile_headers(${PROJECT_NAME} PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:pch.h>")

target_link_libraries(${PROJECT_NAME} PRIVATE glfw glm spdlog)
dynamic_link(${PROJECT_NAME} ../Vendor/libs)