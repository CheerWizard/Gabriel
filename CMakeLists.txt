cmake_minimum_required(VERSION 3.20)

include(cmake_tools/CMakeLists.txt)

project(Gabriel)

set(CMAKE_CXX_STANDARD 17)

# Targets
if(DEBUG)
    add_definitions(-DDEBUG=1)
endif(DEBUG)

if(IMGUI)
    add_definitions(-DIMGUI=1)
endif(IMGUI)

file(GLOB_RECURSE SRC
        cpp/*.cpp
        include/*.h

        vendor/glad-c/src/glad.c

        vendor/stb/stb_image.h

        vendor/imgui/imgui.cpp
        vendor/imgui/imgui_draw.cpp
        vendor/imgui/imgui_tables.cpp
        vendor/imgui/imgui_widgets.cpp
        vendor/imgui/imgui_demo.cpp
        vendor/imgui/backends/imgui_impl_glfw.cpp
        vendor/imgui/backends/imgui_impl_opengl3.cpp
        vendor/imgui/misc/cpp/imgui_stdlib.cpp

        vendor/ImGuizmo/ImGuizmo.cpp
        vendor/ImGuizmo/ImGradient.cpp
        vendor/ImGuizmo/ImSequencer.cpp
        vendor/ImGuizmo/ImCurveEdit.cpp
        vendor/ImGuizmo/GraphEditor.cpp
)

include_directories(
        include
        vendor/glfw/include
        vendor/glad-c/include
        vendor/glm/glm
        vendor/stb
        vendor/assimp/include
        vendor/freetype/include
        vendor/imgui
        vendor/spdlog/include
        vendor/ImGuizmo
)

add_executable(${PROJECT_NAME} ${SRC})
target_precompile_headers(${PROJECT_NAME} PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:pch.h>")

add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glm)
add_subdirectory(vendor/spdlog)

replace_dirs(shaders shaders)
replace_dirs(images images)
replace_dirs(models models)
replace_dirs(fonts fonts)

configure_file(Gabriel_Logo.png Gabriel_Logo.png COPYONLY)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw glm spdlog)
dynamic_link(${PROJECT_NAME} vendor/libs)

if(IMGUI)

    if(DEBUG)
        configure_file(shaders/build_imgui_debug/core.glsl shaders/core.glsl COPYONLY)
    else()
        configure_file(shaders/build_imgui_release/core.glsl shaders/core.glsl COPYONLY)
    endif(DEBUG)

else()

    if(DEBUG)
        configure_file(shaders/build_debug/core.glsl shaders/core.glsl COPYONLY)
    else()
        configure_file(shaders/build_release/core.glsl shaders/core.glsl COPYONLY)
    endif(DEBUG)

endif(IMGUI)