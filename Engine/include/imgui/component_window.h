#pragma once

#include <imgui.h>

namespace gl {

    struct GABRIEL_API ComponentWindow final {

        static const char* title;
        static glm::vec2 position;
        static glm::vec2 resolution;
        static ImGuiWindowFlags windowFlags;

        static void render();

    private:
        static void end();

        static void renderComponents();

        template<typename T>
        static void displayAddComponent(const char* name);

        static void renderTransformComponents();

        static void renderLightComponents();

        static void renderTextComponents();

        static void renderMaterialComponents();

        static void renderOutlineComponent();

        static void renderShadowComponents();

    private:
        static Entity sEntity;
    };

    template<typename T>
    void ComponentWindow::displayAddComponent(const char* name) {
        if (!sEntity.validComponent<T>()) {
            if (ImGui::MenuItem(name)) {
                sEntity.addComponent<T>();
                ImGui::CloseCurrentPopup();
            }
        }
    }

}