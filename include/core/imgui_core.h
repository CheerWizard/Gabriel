#pragma once

#include <core/window.h>

#include <api/image.h>

#include <debugging/visuals.h>

#include <features/screen.h>
#include <features/transform.h>
#include <features/lighting/light_color.h>

#include <control/camera.h>

#include <postfx/hdr.h>
#include <postfx/blur.h>
#include <postfx/bloom.h>
#include <postfx/ssao.h>
#include <postfx/fxaa.h>

#include <text/text.h>

#include <ui/ui.h>

#include <features/lighting/environment.h>

#include <pbr/pbr.h>

#include <imgui/codicons.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <ImGuizmo.h>

#define IMGUI_ID(...) ImguiCore::ID({__VA_ARGS__}).c_str()

namespace gl {

    struct ImguiCoreCallback {
        bool enablePolygonMode = false;
        PolygonFace polygonFace = PolygonFace::FRONT;
        PolygonType polygonType = PolygonType::LINE;

        virtual void resize(int w, int h) = 0;
        virtual void resample(int samples) = 0;
    };

    struct ImguiCore final {

        static ImGuiIO* IO;
        static bool close;
        static ImGuiID dockspaceId;
        static ImGuiDockNodeFlags dockspaceFlags;
        static ImFont* regularFont;
        static ImFont* boldFont;
        static ImguiCoreCallback* callback;
        static bool frameBufferResized;

        static Window* window;
        static Camera* camera;
        static Scene* scene;
        static Environment* environment;
        static Entity selectedEntity;

        static ScreenRenderer* screenRenderer;
        static HdrRenderer* hdrRenderer;
        static BlurRenderer* blurRenderer;
        static BloomRenderer* bloomRenderer;
        static SsaoRenderer* ssaoRenderer;
        static FXAARenderer* fxaaRenderer;

        static TransparentRenderer* transparentRenderer;

        static ShadowPipeline* shadowPipeline;
        static PBR_Pipeline* pbrPipeline;
        static UI_Pipeline* uiPipeline;
        static VisualsPipeline* visualsPipeline;

        static void init(Window *window, const char *shaderLangVersion = "#version 460 core");

        static void free();

        static void setDarkTheme();

        static void begin();

        static void end();

        static void resize(int w, int h);

        static void resample(int samples);

        static void addRegularFont(const char* filepath, float size);

        static void addBoldFont(const char* filepath, float size);

        static void addIconFont(const char* filepath, float size);

        static void setFont(ImFont* font);

        static void setIniFilename(const char *iniFilename);

        static std::string ID(const std::vector<const char *> &str);

        static void selectEntity(const Entity& entity);

        static void unselectEntity();

        static bool Checkbox(const char *label, bool &v, const char *fmt = "%s");

        static bool InputUInt(const char *label, u32 &v, const char *fmt = "%s");

        static bool InputInt(const char *label, int &v, const char *fmt = "%s");

        static bool InputInt2(const char *label, glm::ivec2 &v, const char *fmt = "%s");

        static bool InputInt3(const char *label, glm::ivec3 &v, const char *fmt = "%s");

        static bool InputInt4(const char *label, glm::ivec4 &v, const char *fmt = "%s");

        static bool InputFloat(const char *label, float &v, float step, const char *fmt = "%s");

        static bool InputFloat2(const char *label, glm::fvec2 &v, const char *fmt = "%s");

        static bool InputFloat3(const char *label, glm::fvec3 &v, const char *fmt = "%s");

        static bool InputFloat4(const char *label, glm::fvec4 &v, const char *fmt = "%s");

        static bool DrawVec2Control(
                const std::string &label, glm::vec2 &values,
                const std::array<std::string, 2>& tags = { "X", "Y" },
                float resetValue = 0.0f,
                float columnWidth = 100.0f
        );

        static bool DrawVec3Control(
                const std::string &label, glm::vec3 &values,
                const std::array<std::string, 3>& tags = { "X", "Y", "Z" },
                float resetValue = 0.0f,
                float columnWidth = 100.0f
        );

        static bool DrawVec4Control(
                const std::string &label, glm::vec4 &values,
                const std::array<std::string, 4>& tags = { "X", "Y", "Z", "W" },
                float resetValue = 0.0f,
                float columnWidth = 100.0f
        );

        template<typename T, typename UIFunction>
        static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction);

        static void DrawTransform(Transform& transform);

        static void DrawTransform2d(Transform2d& transform);

        static void DrawColor3Control(
                const std::string &label, glm::vec3 &values,
                float resetValue = 0.0f,
                float columnWidth = 100.0f
        );

        static void DrawColor4Control(
                const std::string &label, glm::vec4 &values,
                float resetValue = 0.0f,
                float columnWidth = 100.0f
        );

        static bool ColorEdit3(const char* label, glm::vec3& values, const char* fmt = "%s");
        static bool ColorEdit4(const char* label, glm::vec4& values, const char* fmt = "%s");

        static void DrawLightColorControl(
                const std::string &label, LightColor& color,
                float resetValue = 0.0f,
                float columnWidth = 100.0f
        );

        static bool InputText(std::string& text);

        static void DrawFontStyle(Style& style);

        static bool IconRadioButton(const char* id, const char* icon, bool& checked);

        static bool IconButton(const char* id, const char* icon);

        static void Spacing(float width, float height);

        static void FullscreenMode();

        static void WindowMode();

    };

    template<typename T, typename UIFunction>
    void ImguiCore::DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction) {
        ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen;
        treeNodeFlags |= ImGuiTreeNodeFlags_Framed;
        treeNodeFlags |= ImGuiTreeNodeFlags_SpanAvailWidth;
        treeNodeFlags |= ImGuiTreeNodeFlags_AllowItemOverlap;
        treeNodeFlags |= ImGuiTreeNodeFlags_FramePadding;

        if (entity.validComponent<T>()) {
            auto& component = *entity.getComponent<T>();
            ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
            float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
            ImGui::Separator();
            bool open = ImGui::TreeNodeEx(name.c_str(), treeNodeFlags);
            ImGui::PopStyleVar();
            ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
            if (ImGui::Button("+", ImVec2 { lineHeight, lineHeight })) {
                ImGui::OpenPopup("ComponentSettings");
            }

            bool removeComponent = false;
            if (ImGui::BeginPopup("ComponentSettings")) {
                if (ImGui::MenuItem("Remove Component"))
                    removeComponent = true;

                ImGui::EndPopup();
            }

            if (open) {
                uiFunction(component);
                ImGui::TreePop();
            }

            if (removeComponent)
                entity.removeComponent<T>();
        }
    }

}
