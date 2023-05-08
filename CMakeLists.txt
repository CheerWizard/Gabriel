cmake_minimum_required(VERSION 3.20)

include(cmake_tools/CMakeLists.txt)

project(Gabriel)

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
)

add_executable(${PROJECT_NAME} ${SRC})

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
)

add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glm)
add_subdirectory(vendor/spdlog)

replace_dirs(shaders shaders)
replace_dirs(images images)
replace_dirs(models models)
replace_dirs(fonts fonts)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw glm spdlog)
dynamic_link(${PROJECT_NAME} vendor/libs)