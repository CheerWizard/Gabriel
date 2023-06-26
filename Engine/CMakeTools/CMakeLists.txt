macro(include_engine path)
    include_directories(
        ${path}/Engine/include
        ${path}/Vendor/glfw/include
        ${path}/Vendor/glad-c/include
        ${path}/Vendor/glm/glm
        ${path}/Vendor/stb
        ${path}/Vendor/assimp/include
        ${path}/Vendor/freetype/include
        ${path}/Vendor/imgui
        ${path}/Vendor/spdlog/include
        ${path}/Vendor/ImGuizmo
    )
endmacro()

macro(include_engine_pch path)
    target_precompile_headers(${PROJECT_NAME} PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${path}/Engine/include/pch.h>")
endmacro()

macro(link_engine project path)
    dynamic_link(${project} ${PROJECT_BINARY_DIR}/${path}/Engine)
endmacro()

macro(build_engine_dll)
    add_definitions(-DGABRIEL_DLL_BUILD=1)
endmacro()

macro(include_engine_dll)
    add_definitions(-DGABRIEL_DLL=1)
endmacro()