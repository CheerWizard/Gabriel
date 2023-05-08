#include <imgui/screen_window.h>

namespace gl {

    static ImageWindow screenImage = { "Screen", InvalidImageBuffer };

    static bool pInitialized = false;

    void ScreenWindow::render() {
        screenImage.imageBuffer = ImguiCore::screenRenderer->getRenderTarget();
        if (!pInitialized) {
            pInitialized = true;
            ImGui::SetNextWindowDockID(ImguiCore::dockspaceId);
        }
        screenImage.render();
    }

}