#pragma once

#include <core/window.h>

#include <api/image.h>

#include <features/screen.h>

#include <control/camera.h>

#include <ecs/scene.h>

#include <postfx/hdr.h>
#include <postfx/blur.h>
#include <postfx/bloom.h>
#include <postfx/ssao.h>

#include <imgui.h>

#define IMGUI_ID(...) ImguiCore::ID({__VA_ARGS__}).c_str()

namespace gl {

    struct ImguiCore final {

        static ImGuiIO* IO;
        static bool close;
        static ImGuiID dockspaceId;
        static ImGuiDockNodeFlags dockspaceFlags;
        static const char* dockLeft;
        static const char* dockRight;
        static const char* dockUp;
        static const char* dockDown;

        static Window* window;
        static Camera* camera;
        static Scene* scene;

        static ScreenRenderer* screenRenderer;
        static HdrRenderer* hdrRenderer;
        static BlurRenderer* blurRenderer;
        static BloomRenderer* bloomRenderer;
        static SsaoRenderer* ssaoRenderer;

        static void init(Window* window, const char* shaderLangVersion = "#version 460 core");
        static void free();

        static void begin();
        static void end();

        static void addFont(const char* filepath, float size);
        static void setIniFilename(const char* iniFilename);

        static std::string ID(const std::vector<const char*>& str);

        static bool Checkbox(const char* label, bool& v, const char* fmt = "%s");

        static bool InputInt(const char* label, int& v, int step, const char* fmt = "%s");
        static bool InputInt2(const char* label, glm::ivec2& v, float step, const char* fmt = "%s");
        static bool InputInt3(const char* label, glm::ivec3& v, float step, const char* fmt = "%s");
        static bool InputInt4(const char* label, glm::ivec4& v, float step, const char* fmt = "%s");

        static bool InputFloat(const char* label, float& v, float step, const char* fmt = "%s");
        static bool InputFloat2(const char* label, glm::fvec2& v, float step, const char* fmt = "%s");
        static bool InputFloat3(const char* label, glm::fvec3& v, float step, const char* fmt = "%s");
        static bool InputFloat4(const char* label, glm::fvec4& v, float step, const char* fmt = "%s");

    };

}