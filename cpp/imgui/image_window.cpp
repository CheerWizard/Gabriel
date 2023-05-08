#include <imgui/image_window.h>

namespace gl {

    void ImageWindow::render() {
        static bool open = true;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

        if (!ImGui::Begin(title, &open)) {
            end();
            return;
        }

        if (!mInitialized) {
            mInitialized = true;
            ImGui::SetWindowPos({ position.x, position.y });
            ImGui::SetWindowSize({ resolution.x, resolution.y });
        }

        ImGui::Image((ImTextureID) imageBuffer.id, ImGui::GetContentRegionAvail(), { 0, 0 }, { 1, 1 });

        end();
    }

    void ImageWindow::end() {
        ImGui::PopStyleVar(2);
        ImGui::End();
    }

}