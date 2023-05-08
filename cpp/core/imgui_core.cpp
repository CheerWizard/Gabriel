#include <core/imgui_core.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif

namespace gl {

    Window* ImguiCore::window;
    ImGuiIO* ImguiCore::IO;
    bool ImguiCore::close = false;

    ImGuiID ImguiCore::dockspaceId;
    ImGuiDockNodeFlags ImguiCore::dockspaceFlags = ImGuiDockNodeFlags_None;

    ScreenRenderer* ImguiCore::screenRenderer = null;
    HdrRenderer* ImguiCore::hdrRenderer = null;
    BlurRenderer* ImguiCore::blurRenderer = null;
    BloomRenderer* ImguiCore::bloomRenderer = null;
    SsaoRenderer* ImguiCore::ssaoRenderer = null;

    Camera* ImguiCore::camera = null;

    Scene* ImguiCore::scene = null;

    void ImguiCore::init(Window* window, const char* shaderLangVersion) {
        ImguiCore::window = window;

        // Setup context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        // Setup input configs
        IO = &ImGui::GetIO();
        IO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        IO->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        IO->ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking Space
        IO->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Viewports
        IO->ConfigDockingWithShift = false;

        // Setup style
        ImGui::StyleColorsLight();

        // Setup backends
        ImGui_ImplGlfw_InitForOpenGL(window->getHandle(), true);
        ImGui_ImplOpenGL3_Init(shaderLangVersion);
    }

    void ImguiCore::free() {
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
        GLFWwindow* backupWindow = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backupWindow);
    }

    void ImguiCore::addFont(const char* filepath, float size) {
        IO->Fonts->AddFontFromFileTTF(filepath, size);
    }

    void ImguiCore::setIniFilename(const char* iniFilename) {
        IO->IniFilename = iniFilename;
    }

    std::string ImguiCore::ID(const std::vector<const char*> &str) {
        std::stringstream ss;
        ss << "##";
        for (auto& s : str) {
            ss << s;
        }
        return ss.str();
    }

    bool ImguiCore::Checkbox(const char* label, bool &v, const char* fmt) {
        ImGui::Text(fmt, label);
        ImGui::SameLine();
        return ImGui::Checkbox(IMGUI_ID(label), &v);
    }

    bool ImguiCore::InputInt(const char* label, int &v, int step, const char* fmt) {
        ImGui::Text(fmt, label);
        ImGui::SameLine();

        bool updated = ImGui::InputInt(IMGUI_ID(label), &v);
        ImGui::SameLine();

        if (ImGui::ArrowButton(IMGUI_ID(label, "_LeftArrow"), ImGuiDir_Left)) {
            v -= step;
            updated = true;
        }
        ImGui::SameLine();

        if (ImGui::ArrowButton(IMGUI_ID(label, "_RightArrow"), ImGuiDir_Right)) {
            v += step;
            updated = true;
        }

        return updated;
    }

    bool ImguiCore::InputFloat(const char* label, float &v, float step, const char *fmt) {
        ImGui::Text(fmt, label);
        ImGui::SameLine();

        bool updated = ImGui::InputFloat(IMGUI_ID(label), &v);
        ImGui::SameLine();

        if (ImGui::ArrowButton(IMGUI_ID(label, "_LeftArrow"), ImGuiDir_Left)) {
            v -= step;
            updated = true;
        }
        ImGui::SameLine();

        if (ImGui::ArrowButton(IMGUI_ID(label, "_RightArrow"), ImGuiDir_Right)) {
            v += step;
            updated = true;
        }

        return updated;
    }

}