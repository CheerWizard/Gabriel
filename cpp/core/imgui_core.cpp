#include <core/imgui_core.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <sstream>

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif

namespace gl {

    Window* ImguiCore::window;
    ImGuiIO* ImguiCore::IO;
    bool ImguiCore::close = false;
    ImFont* ImguiCore::regularFont = null;
    ImFont* ImguiCore::boldFont = null;
    ImguiCoreCallback* ImguiCore::callback = null;

    ImGuiID ImguiCore::dockspaceId;
    ImGuiDockNodeFlags ImguiCore::dockspaceFlags = ImGuiDockNodeFlags_None;

    ScreenRenderer* ImguiCore::screenRenderer = null;
    HdrRenderer* ImguiCore::hdrRenderer = null;
    BlurRenderer* ImguiCore::blurRenderer = null;
    BloomRenderer* ImguiCore::bloomRenderer = null;
    SsaoRenderer* ImguiCore::ssaoRenderer = null;

    UI_Pipeline* ImguiCore::uiPipeline = null;
    Camera* ImguiCore::camera = null;
    Scene* ImguiCore::scene = null;
    Environment* ImguiCore::environment = null;

    Entity ImguiCore::selectedEntity;

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

    void ImguiCore::resize(int w, int h) {
        window->resizeFrame(w, h);
        if (callback)
            callback->resize(w, h);
    }

    void ImguiCore::addRegularFont(const char* filepath, float size) {
        regularFont = IO->Fonts->AddFontFromFileTTF(filepath, size);
    }

    void ImguiCore::addBoldFont(const char* filepath, float size) {
        boldFont = IO->Fonts->AddFontFromFileTTF(filepath, size);
    }

    void ImguiCore::setFont(ImFont *font) {
        IO->FontDefault = font;
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

    bool ImguiCore::InputUInt(const char* label, u32 &v, const char* fmt) {
        ImGui::Text(fmt, label);
        ImGui::SameLine();
        return ImGui::InputInt(IMGUI_ID(label), (int*) &v);
    }

    bool ImguiCore::InputInt(const char* label, int &v, const char* fmt) {
        ImGui::Text(fmt, label);
        ImGui::SameLine();
        return ImGui::InputInt(IMGUI_ID(label), &v);
    }

    bool ImguiCore::InputInt2(const char* label, glm::ivec2 &v, const char *fmt) {
        ImGui::Text(fmt, label);
        ImGui::SameLine();
        return ImGui::InputInt2(IMGUI_ID(label), glm::value_ptr(v));
    }

    bool ImguiCore::InputInt3(const char* label, glm::ivec3 &v, const char *fmt) {
        ImGui::Text(fmt, label);
        ImGui::SameLine();
        return ImGui::InputInt3(IMGUI_ID(label), glm::value_ptr(v));
    }

    bool ImguiCore::InputInt4(const char* label, glm::ivec4 &v, const char *fmt) {
        ImGui::Text(fmt, label);
        ImGui::SameLine();
        return ImGui::InputInt4(IMGUI_ID(label), glm::value_ptr(v));
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

    bool ImguiCore::InputFloat2(const char* label, glm::fvec2 &v, const char *fmt) {
        ImGui::Text(fmt, label);
        ImGui::SameLine();
        return ImGui::InputFloat2(IMGUI_ID(label), glm::value_ptr(v));
    }

    bool ImguiCore::InputFloat3(const char* label, glm::fvec3 &v, const char *fmt) {
        ImGui::Text(fmt, label);
        ImGui::SameLine();
        return ImGui::InputFloat3(IMGUI_ID(label), glm::value_ptr(v));
    }

    bool ImguiCore::InputFloat4(const char* label, glm::fvec4 &v, const char *fmt) {
        ImGui::Text(fmt, label);
        ImGui::SameLine();
        return ImGui::InputFloat4(IMGUI_ID(label), glm::value_ptr(v));
    }

    bool ImguiCore::DrawVec2Control(
            const std::string& label, glm::vec2& values,
            const std::array<std::string, 2>& tags,
            float resetValue, float columnWidth
    ) {
        bool press = false;

        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text("%s", label.c_str());
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button(tags[0].c_str(), buttonSize)) {
            values.x = resetValue;
            press = true;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat(IMGUI_ID("##X", label.c_str()), &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button(tags[1].c_str(), buttonSize)) {
            values.y = resetValue;
            press = true;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat(IMGUI_ID("##Y", label.c_str()), &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();

        ImGui::PopStyleVar();

        ImGui::Columns(1);

        ImGui::PopID();

        return press;
    }

    bool ImguiCore::DrawVec3Control(
            const std::string& label, glm::vec3& values,
            const std::array<std::string, 3>& tags,
            float resetValue, float columnWidth
    ) {
        bool press = false;

        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text("%s", label.c_str());
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button(tags[0].c_str(), buttonSize)) {
            values.x = resetValue;
            press = true;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat(IMGUI_ID("##X", label.c_str()), &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button(tags[1].c_str(), buttonSize)) {
            values.y = resetValue;
            press = true;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat(IMGUI_ID("##Y", label.c_str()), &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button(tags[2].c_str(), buttonSize)) {
            values.z = resetValue;
            press = true;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat(IMGUI_ID("##Z", label.c_str()), &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();

        ImGui::PopStyleVar();

        ImGui::Columns(1);

        ImGui::PopID();

        return press;
    }

    bool ImguiCore::DrawVec4Control(
            const std::string& label, glm::vec4& values,
            const std::array<std::string, 4>& tags,
            float resetValue, float columnWidth
    ) {
        bool press = false;

        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text("%s", label.c_str());
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button(tags[0].c_str(), buttonSize)) {
            values.x = resetValue;
            press = true;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat(IMGUI_ID("##X", label.c_str()), &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button(tags[1].c_str(), buttonSize)) {
            values.y = resetValue;
            press = true;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat(IMGUI_ID("##Y", label.c_str()), &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button(tags[2].c_str(), buttonSize)) {
            values.z = resetValue;
            press = true;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat(IMGUI_ID("##Z", label.c_str()), &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.7f, 0.8f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button(tags[3].c_str(), buttonSize)) {
            values.w = resetValue;
            press = true;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat(IMGUI_ID("##W", label.c_str()), &values.w, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();

        ImGui::PopStyleVar();

        ImGui::Columns(1);

        ImGui::PopID();

        return press;
    }

    void ImguiCore::DrawTransform(Transform& transform) {
        ImguiCore::DrawVec3Control("Translation", transform.translation);
        glm::vec3 rotation = glm::degrees(transform.rotation);
        ImguiCore::DrawVec3Control("Rotation", rotation);
        transform.rotation = glm::radians(rotation);
        ImguiCore::DrawVec3Control("Scale", transform.scale, { "X", "Y", "Z" }, 1.0f);
    }

    void ImguiCore::DrawTransform2d(Transform2d& transform) {
        ImguiCore::DrawVec2Control("Translation", transform.translation);
        ImguiCore::InputFloat("Rotation", transform.rotation, 1.0f);
        ImguiCore::DrawVec2Control("Scale", transform.scale, { "X", "Y" }, 1.0f);
    }

    void ImguiCore::DrawColor3Control(
            const std::string &label, glm::vec3 &values,
            float resetValue, float columnWidth
    ) {
        DrawVec3Control(label, values, { "R", "G", "B" }, resetValue, columnWidth);
    }

    void ImguiCore::DrawColor4Control(
            const std::string &label, glm::vec4 &values,
            float resetValue, float columnWidth
    ) {
        DrawVec4Control(label, values, { "R", "G", "B", "A" }, resetValue, columnWidth);
    }

    bool ImguiCore::ColorEdit3(const char* label, glm::vec3 &values, const char* fmt) {
        ImGui::Text(fmt, label);
        ImGui::SameLine();
        return ImGui::ColorEdit3(IMGUI_ID(label), glm::value_ptr(values));
    }

    bool ImguiCore::ColorEdit4(const char* label, glm::vec4 &values, const char* fmt) {
        ImGui::Text(fmt, label);
        ImGui::SameLine();
        return ImGui::ColorEdit4(IMGUI_ID(label), glm::value_ptr(values));
    }

    void ImguiCore::DrawLightColorControl(const std::string& label, LightColor& color, float resetValue, float columnWidth) {
        ImGui::SeparatorText("Light Color");
        DrawColor3Control("", color.rgb, resetValue, columnWidth);
        ImGui::DragFloat(IMGUI_ID("##light_color", label.c_str()), &color.intensity, 0.1f);
    }

    bool ImguiCore::InputText(std::string& text) {
        char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        strncpy_s(buffer, sizeof(buffer), text.c_str(), sizeof(buffer));
        if (ImGui::InputText(IMGUI_ID("##Tag", text.c_str()), buffer, sizeof(buffer))) {
            text = std::string(buffer);
            return true;
        }
        return false;
    }

    void ImguiCore::DrawFontStyle(Style &style) {
        Font* font = style.font;
        if (font) {
            std::string& fontPath = FontAtlas::fontPaths[font->id];
            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strncpy_s(buffer, sizeof(buffer), fontPath.c_str(), sizeof(buffer));
            if (ImGui::InputText(IMGUI_ID("##Tag", fontPath.c_str()), buffer, sizeof(buffer))) {
                fontPath = std::string(buffer);
            }

            ImGui::SameLine();

            if (ImGui::Button("Save", { 48, 24 })) {
                u32 fontSize = font->size;
                FontAtlas::remove(font->id);
                font = FontAtlas::load(fontPath.c_str(), fontSize);
            }
        }
        style.font = font;

        InputUInt("Size", font->size);
        DrawColor4Control("Color", style.color);
        DrawVec2Control("Padding", style.padding);
    }

}