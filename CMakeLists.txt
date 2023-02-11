cmake_minimum_required(VERSION 3.20.2)
project(CGP)

set(CMAKE_CXX_STANDARD 14)

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
)

add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glm)

configure_file(shaders/solid.vert shaders/solid.vert COPYONLY)
configure_file(shaders/solid.frag shaders/solid.frag COPYONLY)
configure_file(shaders/uv_normal.vert shaders/uv_normal.vert COPYONLY)
configure_file(shaders/uv_normal.frag shaders/uv_normal.frag COPYONLY)

configure_file(images/wall.jpg images/wall.jpg COPYONLY)
configure_file(images/smile.png images/smile.png COPYONLY)
configure_file(images/container.png images/container.png COPYONLY)
configure_file(images/container_specular.png images/container_specular.png COPYONLY)
configure_file(images/container_emission.jpg images/container_emission.jpg COPYONLY)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw glm)