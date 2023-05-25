#include <imgui/image_window.h>
#include <core/imgui_core.h>

namespace gl {

    void ImageWindow::render() {
        begin();
        draw();
        end();
    }

    bool ImageWindow::begin() {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

        if (!ImGui::Begin(title, &show, windowFlags)) {
            end();
            return false;
        }

        if (!mInitialized) {
            mInitialized = true;
            ImGui::SetWindowPos({ position.x, position.y });
            ImGui::SetWindowSize({ resolution.x, resolution.y });
            mCurrentFrameSize = ImGui::GetContentRegionAvail();
        }

        ImVec2 frameSize = ImGui::GetContentRegionAvail();
        if (resizeFramebuffer && (mCurrentFrameSize.x != frameSize.x || mCurrentFrameSize.y != frameSize.y)) {
            ImguiCore::resize(frameSize.x, frameSize.y);
        }
        mCurrentFrameSize = frameSize;

        mHovered = ImGui::IsWindowHovered();

        pollEvents();

        return true;
    }

    void ImageWindow::end() {
        ImGui::PopStyleVar(2);
        ImGui::End();
    }

    void ImageWindow::draw() {
        const auto id = reinterpret_cast<ImTextureID>((unsigned long long) imageBuffer.id);
        ImGui::Image(id, mCurrentFrameSize, { 0, 1 }, { 1, 0 });
    }

    void ImageWindow::pollEvents() {
//        ImguiCore::camera->moveImgui(Timer::getDeltaMillis());
//        ImguiCore::camera->onMouseCursorImgui(ImGui::GetCursorPosX(), ImGui::GetCursorPosY(), Timer::getDeltaMillis());
//
//        if (mScrollX != ImguiCore::IO->MouseWheelH) {
//            mScrollX = ImguiCore::IO->MouseWheelH;
//        }
//
//        if (mScrollY != ImguiCore::IO->MouseWheel) {
//            mScrollY = ImguiCore::IO->MouseWheel;
//            ImguiCore::camera->onMouseScroll(mScrollY, Timer::getDeltaMillis());
//        }
    }

}