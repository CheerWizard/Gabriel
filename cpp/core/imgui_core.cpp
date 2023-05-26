#include <core/imgui_core.h>

#include <imgui/gizmo.h>

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
    ImguiCoreCallback* ImguiCore::callback = null;

    ImFont* ImguiCore::regularFont = null;
    ImFont* ImguiCore::boldFont = null;

    bool ImguiCore::frameBufferResized = false;

    ColorAttachment ImguiCore::logo;

    ImGuiID ImguiCore::dockspaceId;
    ImGuiDockNodeFlags ImguiCore::dockspaceFlags = ImGuiDockNodeFlags_None;

    ScreenRenderer* ImguiCore::screenRenderer = null;
    HdrRenderer* ImguiCore::hdrRenderer = null;
    BlurRenderer* ImguiCore::blurRenderer = null;
    BloomRenderer* ImguiCore::bloomRenderer = null;
    SsaoRenderer* ImguiCore::ssaoRenderer = null;
    FXAARenderer* ImguiCore::fxaaRenderer = null;

    TransparentRenderer* ImguiCore::transparentRenderer = null;

    ShadowPipeline* ImguiCore::shadowPipeline = null;
    PBR_Pipeline* ImguiCore::pbrPipeline = null;
    UI_Pipeline* ImguiCore::uiPipeline = null;
    VisualsPipeline* ImguiCore::visualsPipeline = null;

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

        // Setup backends
        ImGui_ImplGlfw_InitForOpenGL(window->getHandle(), true);
        ImGui_ImplOpenGL3_Init(shaderLangVersion);
    }

    void ImguiCore::free() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void ImguiCore::setDarkTheme() {
        auto& colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_WindowBg] = ColorsF::windowBg;

        // Headers
        colors[ImGuiCol_Header] = ColorsF::header;
        colors[ImGuiCol_HeaderHovered] = ColorsF::headerHover;
        colors[ImGuiCol_HeaderActive] = ColorsF::headerActive;

        // Buttons
        colors[ImGuiCol_Button] = ColorsF::button;
        colors[ImGuiCol_ButtonHovered] = ColorsF::buttonHover;
        colors[ImGuiCol_ButtonActive] = ColorsF::buttonActive;

        // Frame BG
        colors[ImGuiCol_FrameBg] = ColorsF::frameBg;
        colors[ImGuiCol_FrameBgHovered] = ColorsF::frameBgHover;
        colors[ImGuiCol_FrameBgActive] = ColorsF::frameBgActive;

        // Tabs
        colors[ImGuiCol_Tab] = ColorsF::tab;
        colors[ImGuiCol_TabHovered] = ColorsF::tabHover;
        colors[ImGuiCol_TabActive] = ColorsF::tabActive;
        colors[ImGuiCol_TabUnfocused] = ColorsF::tabUnfocused;
        colors[ImGuiCol_TabUnfocusedActive] = ColorsF::tabUnfocusedActive;

        // Title
        colors[ImGuiCol_TitleBg] = ColorsF::titleBg;
        colors[ImGuiCol_TitleBgActive] = ColorsF::titleBgActive;
        colors[ImGuiCol_TitleBgCollapsed] = ColorsF::titleBgCollapsed;

        // Menu Bar
        colors[ImGuiCol_MenuBarBg] = ColorsF::menubarBg;

        // Popup
        colors[ImGuiCol_PopupBg] = ColorsF::popupBg;

        // Scrollbar
        colors[ImGuiCol_ScrollbarBg] = ColorsF::scrollbarBg;
    }

    void ImguiCore::begin() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();
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

    void ImguiCore::resample(int samples) {
        if (callback)
            callback->resample(samples);
    }

    void ImguiCore::addRegularFont(const char* filepath, float size) {
        regularFont = IO->Fonts->AddFontFromFileTTF(filepath, size);
    }

    void ImguiCore::addBoldFont(const char* filepath, float size) {
        boldFont = IO->Fonts->AddFontFromFileTTF(filepath, size);
    }

    void ImguiCore::addIconFont(const char* filepath, float size) {
        static const ImWchar iconsRange[] = { ICON_MIN_CI, ICON_MAX_CI, 0 };

        ImFontConfig iconsConfig;
        iconsConfig.MergeMode = true;
        iconsConfig.PixelSnapH = true;

        IO->Fonts->AddFontFromFileTTF(filepath, size, &iconsConfig, iconsRange);
    }

    void ImguiCore::setFont(ImFont *font) {
        IO->FontDefault = font;
    }

    void ImguiCore::setIniFilename(const char* iniFilename) {
        IO->IniFilename = iniFilename;
    }

    void ImguiCore::loadLogo(const char* filepath, const glm::vec2& size) {
        if (logo.buffer.id != InvalidImageBuffer || !logo.image.pixels) {
            logo.free();
        }

        logo.image = ImageReader::read(filepath, true);
        logo.image.resize(size.x, size.y);
        logo.image.internalFormat = GL_RGBA8;
        logo.image.pixelFormat = GL_RGBA;
        logo.image.pixelType = PixelType::U8;

        logo.params.minFilter = GL_LINEAR;
        logo.params.magFilter = GL_LINEAR;
        logo.params.r = GL_CLAMP_TO_EDGE;
        logo.params.s = GL_CLAMP_TO_EDGE;
        logo.params.t = GL_CLAMP_TO_EDGE;

        logo.init();
    }

    std::string ImguiCore::ID(const std::vector<const char*> &str) {
        std::stringstream ss;
        ss << "##";
        for (auto& s : str) {
            ss << s;
        }
        return ss.str();
    }

    void ImguiCore::selectEntity(const Entity& entity) {
        unselectEntity();
        selectedEntity = entity;

        bool selectedDrawable = selectedEntity.validComponent<DrawableElements>();
        bool selectedTransform = selectedEntity.validComponent<Transform>();
        bool selectedPhongLight = selectedEntity.validComponent<PhongLightComponent>();
        bool selectedDirectLight = selectedEntity.validComponent<DirectLightComponent>();
        bool selectedPointLight = selectedEntity.validComponent<PointLightComponent>();
        bool selectedSpotLight = selectedEntity.validComponent<SpotLightComponent>();

        if (selectedEntity.valid()) {

            if (selectedDrawable) {
                selectedEntity.addComponent<PolygonVisual>();
            }

            if (selectedTransform) {
                selectedEntity.addComponent<GizmoTransformComponent>();
            }

            if (selectedPhongLight) {
                selectedEntity.addComponent<LightVisual>();
                selectedEntity.addComponent<GizmoPhongLight>();
            }

            if (selectedDirectLight) {
                selectedEntity.addComponent<LightVisual>();
                selectedEntity.addComponent<GizmoDirectLight>();
            }

            if (selectedPointLight) {
                selectedEntity.addComponent<LightVisual>();
                selectedEntity.addComponent<GizmoPointLight>();
            }

            if (selectedSpotLight) {
                selectedEntity.addComponent<LightVisual>();
                selectedEntity.addComponent<GizmoSpotLight>();
            }
        }
    }

    void ImguiCore::unselectEntity() {
        if (selectedEntity.valid()) {
            selectedEntity.removeComponent<PolygonVisual>();
            selectedEntity.removeComponent<GizmoTransformComponent>();
            selectedEntity.removeComponent<LightVisual>();
            selectedEntity.removeComponent<GizmoPhongLight>();
            selectedEntity.removeComponent<GizmoDirectLight>();
            selectedEntity.removeComponent<GizmoPointLight>();
            selectedEntity.removeComponent<GizmoSpotLight>();
        }
    }

    static bool pEnableInput = true;

    void ImguiCore::enableInput() {
        if (pEnableInput) return;
        pEnableInput = true;

        IO->ConfigFlags &= ~ImGuiConfigFlags_NoMouse;          // Enable Mouse
        IO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard
    }

    void ImguiCore::disableInput() {
        if (!pEnableInput) return;
        pEnableInput = false;

        IO->ConfigFlags |= ImGuiConfigFlags_NoMouse;            // Disable Mouse
        IO->ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard; // Disable Keyboard
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
        glm::vec2 oldValue = values;

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

        return press || (oldValue != values);
    }

    bool ImguiCore::DrawVec3Control(
            const std::string& label, glm::vec3& values,
            const std::array<std::string, 3>& tags,
            float resetValue, float columnWidth
    ) {
        bool press = false;
        glm::vec3 oldValue = values;

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

        return press || (oldValue != values);
    }

    bool ImguiCore::DrawVec4Control(
            const std::string& label, glm::vec4& values,
            const std::array<std::string, 4>& tags,
            float resetValue, float columnWidth
    ) {
        bool press = false;
        glm::vec4 oldValue = values;

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

        return press || (oldValue != values);
    }

    void ImguiCore::DrawTransform(Transform& transform) {
        bool translated = ImguiCore::DrawVec3Control("Translation", transform.translation);

        glm::vec3 rotation = glm::degrees(transform.rotation);
        bool rotated = ImguiCore::DrawVec3Control("Rotation", rotation);
        transform.rotation = glm::radians(rotation);

        bool scaled = ImguiCore::DrawVec3Control("Scale", transform.scale, { "X", "Y", "Z" }, 1.0f);

        if (translated || rotated || scaled) {
            transform.init();
        }
    }

    void ImguiCore::DrawTransform2d(Transform2d& transform) {
        bool translated = ImguiCore::DrawVec2Control("Translation", transform.translation);
        bool rotated = ImguiCore::InputFloat("Rotation", transform.rotation, 1.0f);
        bool scaled = ImguiCore::DrawVec2Control("Scale", transform.scale, { "X", "Y" }, 1.0f);

        if (translated || rotated || scaled) {
            transform.init();
        }
    }

    bool ImguiCore::DrawColor3Control(
            const std::string &label, glm::vec3 &values,
            float resetValue, float columnWidth
    ) {
        return DrawVec3Control(label, values, { "R", "G", "B" }, resetValue, columnWidth);
    }

    bool ImguiCore::DrawColor4Control(
            const std::string &label, glm::vec4 &values,
            float resetValue, float columnWidth
    ) {
        return DrawVec4Control(label, values, { "R", "G", "B", "A" }, resetValue, columnWidth);
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

    bool ImguiCore::DrawLightColorControl(const std::string& label, LightColor& color, float resetValue, float columnWidth) {
        ImGui::SeparatorText("Light Color");
        bool colorChanged = DrawColor3Control("", color.rgb, resetValue, columnWidth);
        ImGui::PushID(IMGUI_ID("##light_intensity_", label.c_str()));
        bool intensityChanged = ImGui::SliderFloat("Intensity", &color.intensity, 0, 100);
        ImGui::PopID();
        return colorChanged || intensityChanged;
    }

    bool ImguiCore::InputText(std::string& text) {
        char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        strncpy_s(buffer, sizeof(buffer), text.c_str(), sizeof(buffer));

        if (ImGui::InputText("##tag", buffer, sizeof(buffer))) {
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

    bool ImguiCore::IconRadioButton(const char* id, const char* icon, bool& checked, const ImVec2& size, const float cornerRadius) {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, cornerRadius);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, Color::LAVENDER_GREY);
        ImGui::PushID(id);

        if (checked) {
            ImGui::PushStyleColor(ImGuiCol_Button, Color::LAVENDER_GREY);
            ImGui::PushStyleColor(ImGuiCol_Text, Color::BLACK_PEARL);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Color::ALICE_BLUE);
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button, Color::CORNFLOWER_BLUE);
            ImGui::PushStyleColor(ImGuiCol_Text, Color::LAVENDER_GREY);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Color::JORDY_BLUE);
        }

        bool pressed = ImGui::Button(icon, size);
        if (pressed) {
            checked = !checked;
        }

        ImGui::PopID();
        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar();

        return pressed;
    }

    bool ImguiCore::IconButton(
            const char* id,
            const char* icon,
            const ImVec2& size,
            const float cornerRadius,
            const u32 baseColor,
            const u32 hoverColor,
            const u32 activeColor,
            const u32 textColor
    ) {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, cornerRadius);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, activeColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoverColor);
        ImGui::PushStyleColor(ImGuiCol_Button, baseColor);
        ImGui::PushStyleColor(ImGuiCol_Text, textColor);
        ImGui::PushID(id);

        bool pressed = ImGui::Button(icon, size);

        ImGui::PopID();
        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar();

        return pressed;
    }

    void ImguiCore::Spacing(float width, float height) {
        ImGui::Dummy({ width, height });
    }

    void ImguiCore::FullscreenMode() {
        float width = static_cast<float>(window->getMonitorWidth());
        float height = static_cast<float>(window->getMonitorHeight());
        ImGui::SetWindowPos({0,0 });
        ImGui::SetWindowSize({ width, height });
        window->resizeFrame(width, height);
    }

    void ImguiCore::WindowMode() {
        float width = static_cast<float>(window->getWidth());
        float height = static_cast<float>(window->getHeight());
        ImGui::SetWindowPos({0, 0 });
        ImGui::SetWindowSize({ width, height });
        window->resizeFrame(width, height);
    }

    void ImguiCore::FullscreenWindowMode() {
        float width = static_cast<float>(window->getMonitorWidth());
        float height = static_cast<float>(window->getMonitorHeight());
        ImGui::SetWindowPos({0, 0 });
        ImGui::SetWindowSize({ width, height });
        window->resizeFrame(width, height);
    }

    bool ImguiCore::SliderFloat(const char *label, float &value, const float min, const float max) {
        ImGui::PushID(IMGUI_ID(label));
        bool result = ImGui::SliderFloat(label, &value, min, max);
        ImGui::PopID();
        return result;
    }

    bool ImguiCore::SliderInt(const char *label, int &value, const int min, const int max) {
        ImGui::PushID(IMGUI_ID(label));
        bool result = ImGui::SliderInt(label, &value, min, max);
        ImGui::PopID();
        return result;
    }

}