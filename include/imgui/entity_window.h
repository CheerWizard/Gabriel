#pragma once

#include <imgui.h>

namespace gl {

    struct EntityWindow final {

        static const char* title;
        static glm::vec2 position;
        static glm::vec2 resolution;
        static ImGuiWindowFlags windowFlags;

        static void render();

    private:
        static void end();
        static void renderEntity(Entity entity);
    };

}