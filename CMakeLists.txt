cmake_minimum_required(VERSION 3.23)
project(CGP)

set(CMAKE_CXX_STANDARD 14)

file(GLOB_RECURSE SRC cpp/*.cpp include/*.h vendor/glad-c/src/glad.c)

add_executable(${PROJECT_NAME} ${SRC} cpp/vertex.cpp include/buffers.h cpp/buffers.cpp include/geometry.h cpp/geometry.cpp include/file_utils.h cpp/file_utils.cpp include/shader.h cpp/shader.cpp include/draw.h cpp/draw.cpp)

include_directories(
        include
        vendor/glfw/include
        vendor/glad-c/include
        vendor/glm/glm
)

add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glm)

configure_file(shaders/triangle.vert shaders/triangle.vert COPYONLY)
configure_file(shaders/triangle.frag shaders/triangle.frag COPYONLY)
configure_file(shaders/rect.vert shaders/rect.vert COPYONLY)
configure_file(shaders/rect.frag shaders/rect.frag COPYONLY)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw glm)