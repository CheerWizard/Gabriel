cmake_minimum_required(VERSION 3.20.2)
project(CGP)

set(CMAKE_CXX_STANDARD 14)

include(cmake_utils/CMakeLists.txt)

file(GLOB_RECURSE SRC
        cpp/*.cpp include/*.h
        vendor/glad-c/src/glad.c
        vendor/stb/stb_image.h
)

add_executable(${PROJECT_NAME} ${SRC})

include_directories(
        include
        vendor/glfw/include
        vendor/glad-c/include
        vendor/glm/glm
        vendor/stb
        vendor/assimp/include
)

add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glm)

configure_file(libs/assimp-vc143-mtd.lib assimp-vc143-mtd.lib COPYONLY)
configure_file(libs/assimp-vc143-mtd.dll assimp-vc143-mtd.dll COPYONLY)

replace_dirs(shaders shaders)
replace_dirs(images images)
replace_dirs(models models)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw glm assimp-vc143-mtd)