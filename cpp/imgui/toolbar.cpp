#include <imgui/toolbar.h>

#include <imgui_internal.h>

namespace gl {

    static ImageWindow hdrImage = { "HDR Viewport", InvalidImageBuffer };
    static ImageWindow blurImage = { "Blur Viewport", InvalidImageBuffer };
    static ImageWindow bloomImage = { "Bloom Viewport", InvalidImageBuffer };
    static ImageWindow ssaoImage = { "SSAO Viewport", InvalidImageBuffer };

    void Viewports::render() {
        if (hdrImage.show) {
            hdrImage.imageBuffer = ImguiCore::hdrRenderer->getRenderTarget();
            hdrImage.render();
        }

        if (blurImage.show) {
            blurImage.imageBuffer = ImguiCore::blurRenderer->getRenderTarget();
            blurImage.render();
        }

        if (bloomImage.show) {
            bloomImage.imageBuffer = ImguiCore::bloomRenderer->getRenderTarget();
            bloomImage.render();
        }

        if (ssaoImage.show) {
            ssaoImage.imageBuffer = ImguiCore::ssaoRenderer->getRenderTarget();
            ssaoImage.render();
        }
    }

    void ViewportsMenu::render() {
        if (ImGui::BeginMenu("Viewports")) {
            ImGui::MenuItem("HDR", null, &hdrImage.show);
            ImGui::MenuItem("Blur", null, &blurImage.show);
            ImGui::MenuItem("Bloom", null, &bloomImage.show);
            ImGui::MenuItem("SSAO", null, &ssaoImage.show);
            ImGui::EndMenu();
        }
    }

    void Toolbar::render() {
        static bool open = false;

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        windowFlags |= ImGuiWindowFlags_NoBackground;

        ImGui::Begin("DockSpace", &open, windowFlags);
        ImGuiID dsId = ImGui::GetID("Dashboard");
        ImguiCore::dockspaceId = dsId;
        if (!ImGui::DockBuilderGetNode(dsId)) {
            ImGui::DockBuilderRemoveNode(dsId);
            ImGui::DockBuilderAddNode(dsId);

            ImGuiID dsIdCopy = dsId;
            ImGuiID dsIdLeft = ImGui::DockBuilderSplitNode(dsIdCopy, ImGuiDir_Left, 0.0f, NULL, &dsIdCopy);
            ImGuiID dsIdRight = ImGui::DockBuilderSplitNode(dsIdCopy, ImGuiDir_Right, 0.0f, NULL, &dsIdCopy);
            ImGuiID dsIdUp = ImGui::DockBuilderSplitNode(dsIdCopy, ImGuiDir_Up, 0.0f, NULL, &dsIdCopy);
            ImGuiID dsIdDown = ImGui::DockBuilderSplitNode(dsIdCopy, ImGuiDir_Down, 0.0f, NULL, &dsIdCopy);

            if (ImguiCore::dockLeft) {
                ImGui::DockBuilderDockWindow(ImguiCore::dockLeft, dsIdLeft);
            }

            if (ImguiCore::dockRight) {
                ImGui::DockBuilderDockWindow(ImguiCore::dockRight, dsIdRight);
            }

            if (ImguiCore::dockUp) {
                ImGui::DockBuilderDockWindow(ImguiCore::dockUp, dsIdUp);
            }

            if (ImguiCore::dockDown) {
                ImGui::DockBuilderDockWindow(ImguiCore::dockDown, dsIdDown);
            }

            ImGui::DockBuilderFinish(dsId);
        }
        ImGui::DockSpace(dsId, ImVec2(0.0f, 0.0f), ImguiCore::dockspaceFlags);

        if (ImGui::BeginMenuBar()) {

            if (ImGui::BeginMenu("Docking")) {
                if (ImGui::MenuItem("NoSplit Mode", null, (ImguiCore::dockspaceFlags & ImGuiDockNodeFlags_NoSplit) != 0)) {
                    ImguiCore::dockspaceFlags ^= ImGuiDockNodeFlags_NoSplit;
                }

                if (ImGui::MenuItem("NoResize Mode", null, (ImguiCore::dockspaceFlags & ImGuiDockNodeFlags_NoResize) != 0)) {
                    ImguiCore::dockspaceFlags ^= ImGuiDockNodeFlags_NoResize;
                }

                if (ImGui::MenuItem("Auto Hide TabBar", null, (ImguiCore::dockspaceFlags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) {
                    ImguiCore::dockspaceFlags ^= ImGuiDockNodeFlags_AutoHideTabBar;
                }

                ImGui::Separator();

                ImguiCore::close = ImGui::MenuItem("Exit", null, false);

                ImGui::EndMenu();
            }

            ViewportsMenu::render();

            ImGui::EndMenuBar();
        }

        ImGui::PopStyleVar(3);
        ImGui::End();

        Viewports::render();
    }

}