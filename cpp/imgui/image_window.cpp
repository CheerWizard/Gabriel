#include <imgui/image_window.h>
#include <core/imgui_core.h>

namespace gl {

    void ImageWindow::render() {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

        if (!ImGui::Begin(title, &show, windowFlags)) {
            end();
            return;
        }

        if (!mInitialized) {
            mInitialized = true;
            ImGui::SetWindowPos({ position.x, position.y });
            ImGui::SetWindowSize({ resolution.x, resolution.y });
            mCurrentFrameSize = ImGui::GetContentRegionAvail();
        }

        ImVec2 frameSize = ImGui::GetContentRegionAvail();
        if (mCurrentFrameSize.x != frameSize.x || mCurrentFrameSize.y != frameSize.y) {
            ImguiCore::resize(frameSize.x, frameSize.y);
        }
        mCurrentFrameSize = frameSize;

        const auto id = reinterpret_cast<ImTextureID>((unsigned long long) imageBuffer.id);
        ImGui::Image(id, frameSize, { 0, 1 }, { 1, 0 });

        end();
    }

    void ImageWindow::end() {
        ImGui::PopStyleVar(2);
        ImGui::End();
    }

}