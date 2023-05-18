#include <core/imgui_core.h>
#include <imgui/screen_window.h>
#include <imgui/image_window.h>

namespace gl {

    static ImageWindow screenImage = { "Screen", InvalidImageBuffer };

    static bool pInitialized = false;

    void ScreenWindow::render() {
        screenImage.imageBuffer = ImguiCore::screenRenderer->getRenderTarget();
        if (!pInitialized) {
            pInitialized = true;
            screenImage.windowFlags |= ImGuiWindowFlags_NoCollapse;
            screenImage.windowFlags |= ImGuiWindowFlags_NoScrollbar;
            screenImage.windowFlags |= ImGuiWindowFlags_DockNodeHost;
            screenImage.show = true;
            ImGui::SetNextWindowDockID(ImguiCore::dockspaceId);
        }
        screenImage.render();
    }

}