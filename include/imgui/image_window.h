#pragma once

#include <api/image.h>

#include <imgui.h>

namespace gl {

    struct ImageWindow final {
        const char* title;
        glm::vec2 resolution;
        glm::vec2 position;
        ImageBuffer imageBuffer;
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None;
        bool show = false;

        ImageWindow(
                const char* title,
                const ImageBuffer& imageBuffer = InvalidImageBuffer,
                const glm::vec2& resolution = { 512, 512 },
                const glm::vec2& position = { 0, 0 }
        ) : title(title), imageBuffer(imageBuffer), resolution(resolution), position(position) {}

        void render();

    private:
        void end();

    private:
        bool mInitialized = false;
        ImVec2 mCurrentFrameSize = { 800, 600 };
    };

}