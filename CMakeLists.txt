cmake_minimum_required(VERSION 3.20)

project(Gabriel)

# Targets
if(DEBUG)
    add_definitions(-DDEBUG=1)
endif(DEBUG)

if(IMGUI)
    add_definitions(-DIMGUI=1)
endif(IMGUI)

add_subdirectory(Vendor/glfw)
add_subdirectory(Vendor/glm)
add_subdirectory(Vendor/spdlog)
add_subdirectory(Engine)
add_subdirectory(Editor)
add_subdirectory(NetworkClient)
add_subdirectory(NetworkServer)