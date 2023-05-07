#pragma once

#include <core/window.h>

#include <imgui.h>

namespace gl {

    struct ImguiCore final {

        ImguiCore(Window* window, const char* shaderLangVersion = "#version 460 core");
        ~ImguiCore();

        void begin();
        void end();

        void addFont(const char* filepath, float size);
        void setIniFilename(const char* iniFilename);

    private:
        Window* mWindow;
        ImGuiIO* mIO;
    };

}