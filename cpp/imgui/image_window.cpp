#include <imgui/image_window.h>

namespace gl {

    void ImageWindow::render() {
        static bool open = true;
        if (!ImGui::Begin(title, &open)) {
            ImGui::End();
            return;
        }

        if (!mInitialized) {
            mInitialized = true;
            ImGui::SetWindowPos({ position.x, position.y });
            ImGui::SetWindowSize({ resolution.x, resolution.y });
        }

        ImGui::Image((ImTextureID) imageBuffer.id, ImGui::GetContentRegionAvail(), { 1, 1 }, { 0, 0 });

        ImGui::End();
    }

}