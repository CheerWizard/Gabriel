#pragma once

#include <api/image.h>

#include <imgui.h>

namespace gl {

    struct GABRIEL_API ImageWindow final {
        const char* title;
        glm::vec2 resolution;
        glm::vec2 position;
        ImageBuffer imageBuffer;
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None;
        bool show = false;
        bool resizeFramebuffer = false;

        ImageWindow(
                const char* title,
                const ImageBuffer& imageBuffer = InvalidImageBuffer,
                const glm::vec2& resolution = { 512, 512 },
                const glm::vec2& position = { 0, 0 }
        ) : title(title), imageBuffer(imageBuffer), resolution(resolution), position(position) {}

        void render();

        bool begin();
        void end();
        void draw();

    private:
        void pollEvents();

    private:
        bool mInitialized = false;
        ImVec2 mCurrentFrameSize = { 800, 600 };
        bool mFocused = false;
        float mScrollX;
        float mScrollY;
    };

}