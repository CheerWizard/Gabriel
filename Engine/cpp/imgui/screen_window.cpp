#include <core/imgui_core.h>

#include <imgui/screen_window.h>
#include <imgui/image_window.h>
#include <imgui/gizmo.h>

namespace gl {

    static ImageWindow screenImage = { "Screen", InvalidImageBuffer };

    static bool pInitialized = false;

    void ScreenWindow::render() {
        screenImage.imageBuffer = ImguiCore::screenRenderer->getRenderTarget();
        if (!pInitialized) {
            pInitialized = true;
            screenImage.windowFlags |= ImGuiWindowFlags_NoTitleBar;
            screenImage.windowFlags |= ImGuiWindowFlags_NoScrollbar;
            screenImage.windowFlags |= ImGuiWindowFlags_NoCollapse;
            screenImage.windowFlags |= ImGuiWindowFlags_DockNodeHost;
            screenImage.show = true;
            screenImage.resizeFramebuffer = true;
            ImGui::SetNextWindowDockID(ImguiCore::dockspaceId);
        }

        if (screenImage.begin()) {
            renderManipulateTab();

            screenImage.draw();

            if (ImguiCore::selectedEntity.valid()) {
                Gizmo::render(ImguiCore::selectedEntity);
            }

            screenImage.end();
        }
    }

    void ScreenWindow::renderManipulateTab() {
        ImguiCore::Spacing(ImGui::GetWindowWidth(), 2);
        ImguiCore::Spacing(ImGui::GetWindowWidth() * 0.5f - 100.0f, 0);
        ImGui::SameLine();

        ImguiCore::IconRadioButton("##world_space", ICON_CI_GLOBE, Gizmo::enableWorldMode);

        ImGui::SameLine(0, 4);

        ImguiCore::IconRadioButton("##translate_mode", ICON_CI_MOVE, Gizmo::enableTranslation);

        ImGui::SameLine(0, 4);

        ImguiCore::IconRadioButton("##rotate_mode", ICON_CI_CIRCLE_LARGE_OUTLINE, Gizmo::enableRotation);

        ImGui::SameLine(0, 4);

        ImguiCore::IconRadioButton("##scale_mode", ICON_CI_ARROW_SWAP, Gizmo::enableScale);

        ImGui::SameLine(0, 4);

        static bool enable_play = false;
        if (enable_play) {
            ImguiCore::IconRadioButton("##pause", ICON_CI_DEBUG_PAUSE, enable_play);
            ImguiCore::callback->enableSimulation = true;
        } else {
            ImguiCore::IconRadioButton("##play", ICON_CI_PLAY, enable_play);
            ImguiCore::callback->enableSimulation = false;
        }

        ImGui::SameLine(0, 4);

        if (ImguiCore::IconButton("##compile", ICON_CI_SYNC)) {
            // compile script code
        }

        ImGui::SameLine(0, 4);

        static bool fullscreen = false;
        if (ImguiCore::IconButton("##fullscreen", ICON_CI_SCREEN_FULL)) {
            fullscreen = !fullscreen;
            if (fullscreen) {
                ImguiCore::FullscreenWindowMode();
            } else {
                ImguiCore::WindowMode();
            }
        }
    }

}