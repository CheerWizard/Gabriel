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
#define IM_COLF(r, g, b, a) ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f)
#define IM_COL(colorF) IM_COL32(colorF.x * 255.0f, colorF.y * 255.0f, colorF.z * 255.0f, colorF.w * 255.0f)

namespace gl {

    struct Color final {
        static constexpr auto LAVENDER_GREY = IM_COL32(205, 209, 228, 255);
        static constexpr auto BLACK_PEARL = IM_COL32(8, 14, 44, 255);
        static constexpr auto JORDY_BLUE = IM_COL32(137, 196, 244, 255);
        static constexpr auto CORNFLOWER_BLUE = IM_COL32(72, 113, 247, 255);
        static constexpr auto ALICE_BLUE = IM_COL32(228, 241, 254, 255);
        static constexpr auto TALL_POPPY = IM_COL32(192, 57, 43, 255);
        static constexpr auto CHESNUT_ROSE = IM_COL32(210, 77, 87, 255);
        static constexpr auto LYNCH = IM_COL32(108, 122, 137, 255);
        static constexpr auto SILVER_SAND = IM_COL32(189, 195, 199, 255);
        static constexpr auto IRON = IM_COL32(218, 223, 225, 255);
    };

    struct ColorF final {
        static constexpr auto LAVENDER_GREY = IM_COLF(205, 209, 228, 255);
        static constexpr auto BLACK_PEARL = IM_COLF(8, 14, 44, 255);
        static constexpr auto JORDY_BLUE = IM_COLF(137, 196, 244, 255);
        static constexpr auto CORNFLOWER_BLUE = IM_COLF(72, 113, 247, 255);
        static constexpr auto ALICE_BLUE = IM_COLF(228, 241, 254, 255);
        static constexpr auto TALL_POPPY = IM_COLF(192, 57, 43, 255);
        static constexpr auto CHESNUT_ROSE = IM_COLF(210, 77, 87, 255);
        static constexpr auto LYNCH = IM_COLF(108, 122, 137, 255);
        static constexpr auto SILVER_SAND = IM_COLF(189, 195, 199, 255);
        static constexpr auto IRON = IM_COLF(218, 223, 225, 255);
    };

    struct Colors final {
        static constexpr auto titleBg = Color::BLACK_PEARL;
        static constexpr auto titleBgActive = Color::LYNCH;
        static constexpr auto titleBgCollapsed = Color::SILVER_SAND;

        static constexpr auto windowBg = Color::BLACK_PEARL;

        static constexpr auto frameBg = Color::BLACK_PEARL;
        static constexpr auto frameBgHover = Color::SILVER_SAND;
        static constexpr auto frameBgActive = Color::IRON;

        static constexpr auto header = Color::BLACK_PEARL;
        static constexpr auto headerHover = Color::LYNCH;
        static constexpr auto headerActive = Color::SILVER_SAND;

        static constexpr auto button = Color::CORNFLOWER_BLUE;
        static constexpr auto buttonHover = Color::JORDY_BLUE;
        static constexpr auto buttonActive = Color::LAVENDER_GREY;

        static constexpr auto tab = Color::JORDY_BLUE;
        static constexpr auto tabHover = Color::LAVENDER_GREY;
        static constexpr auto tabActive = Color::CORNFLOWER_BLUE;
        static constexpr auto tabUnfocused = Color::JORDY_BLUE;
        static constexpr auto tabUnfocusedActive = Color::CORNFLOWER_BLUE;

        static constexpr auto menubarBg = Color::BLACK_PEARL;

        static constexpr auto popupBg = Color::BLACK_PEARL;

        static constexpr auto scrollbarBg = Color::IRON;
    };

    struct ColorsF final {
        static constexpr auto titleBg = ColorF::BLACK_PEARL;
        static constexpr auto titleBgActive = ColorF::LYNCH;
        static constexpr auto titleBgCollapsed = ColorF::SILVER_SAND;

        static constexpr auto windowBg = ColorF::BLACK_PEARL;

        static constexpr auto frameBg = ColorF::BLACK_PEARL;
        static constexpr auto frameBgHover = ColorF::SILVER_SAND;
        static constexpr auto frameBgActive = ColorF::IRON;

        static constexpr auto header = ColorF::BLACK_PEARL;
        static constexpr auto headerHover = ColorF::LYNCH;
        static constexpr auto headerActive = ColorF::SILVER_SAND;

        static constexpr auto button = ColorF::CORNFLOWER_BLUE;
        static constexpr auto buttonHover = ColorF::JORDY_BLUE;
        static constexpr auto buttonActive = ColorF::LAVENDER_GREY;

        static constexpr auto tab = ColorF::JORDY_BLUE;
        static constexpr auto tabHover = ColorF::LAVENDER_GREY;
        static constexpr auto tabActive = ColorF::CORNFLOWER_BLUE;
        static constexpr auto tabUnfocused = ColorF::JORDY_BLUE;
        static constexpr auto tabUnfocusedActive = ColorF::CORNFLOWER_BLUE;

        static constexpr auto menubarBg = ColorF::BLACK_PEARL;

        static constexpr auto popupBg = ColorF::BLACK_PEARL;

        static constexpr auto scrollbarBg = ColorF::IRON;
    };

    struct ImguiCoreCallback {
        bool enablePolygonMode = false;
        PolygonFace polygonFace = PolygonFace::FRONT;
        PolygonType polygonType = PolygonType::LINE;

        bool enableSimulation = false;

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

        static ColorAttachment logo;

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

        static void loadLogo(const char* filepath, const glm::vec2& size = { 48, 48 });

        static std::string ID(const std::vector<const char *> &str);

        static void selectEntity(const Entity& entity);

        static void unselectEntity();

        static void enableInput();

        static void disableInput();

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

        static bool DrawColor3Control(
                const std::string &label, glm::vec3 &values,
                float resetValue = 0.0f,
                float columnWidth = 100.0f
        );

        static bool DrawColor4Control(
                const std::string &label, glm::vec4 &values,
                float resetValue = 0.0f,
                float columnWidth = 100.0f
        );

        static bool ColorEdit3(const char* label, glm::vec3& values, const char* fmt = "%s");
        static bool ColorEdit4(const char* label, glm::vec4& values, const char* fmt = "%s");

        static bool DrawLightColorControl(
                const std::string &label, LightColor& color,
                float resetValue = 0.0f,
                float columnWidth = 100.0f
        );

        static bool InputText(std::string& text);

        static void DrawFontStyle(Style& style);

        static bool IconRadioButton(const char* id, const char* icon, bool& checked, const ImVec2& size = { 24, 24 }, const float cornerRadius = 2.0f);

        static bool IconButton(
                const char* id,
                const char* icon,
                const ImVec2& size = { 24, 24 },
                const float cornerRadius = 2.0f,
                const u32 baseColor = Color::CORNFLOWER_BLUE,
                const u32 hoverColor = Color::JORDY_BLUE,
                const u32 activeColor = Color::LAVENDER_GREY,
                const u32 textColor = Color::LAVENDER_GREY
        );

        static void Spacing(float width, float height);

        static void FullscreenMode();

        static void WindowMode();

        static void FullscreenWindowMode();

        static bool SliderFloat(
                const char* label,
                float& value,
                const float min,
                const float max
        );

        static bool SliderInt(
                const char* label,
                int& value,
                const int min,
                const int max
        );

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

            ImGui::SameLine(contentRegionAvailable.x - 24);
            if (ImguiCore::IconButton("##component_settings", "+")) {
                ImGui::OpenPopup("ComponentSettings");
            }

            bool removeComponent = false;
            if (ImGui::BeginPopup("ComponentSettings")) {
                if (ImGui::MenuItem("Remove"))
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
