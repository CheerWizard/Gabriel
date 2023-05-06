#include <core/imgui_core.h>

#include <api/commands.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif

namespace gl {

    ImguiCore::ImguiCore(Window* window, const char* shaderLangVersion) : mWindow(window) {
        // Setup context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        // Setup input configs
        mIO = &ImGui::GetIO();
        mIO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        mIO->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup style
        ImGui::StyleColorsLight();

        // Setup backends
        ImGui_ImplGlfw_InitForOpenGL(window->getHandle(), false);
        ImGui_ImplOpenGL3_Init(shaderLangVersion);
    }

    ImguiCore::~ImguiCore() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void ImguiCore::begin() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ImguiCore::end() {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void ImguiCore::addFont(const char* filepath, float size) {
        mIO->Fonts->AddFontFromFileTTF(filepath, size);
    }

    void ImguiCore::setIniFilename(const char* iniFilename) {
        mIO->IniFilename = iniFilename;
    }

}