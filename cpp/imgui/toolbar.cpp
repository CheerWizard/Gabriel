#include <imgui/toolbar.h>

#include <imgui_internal.h>

namespace gl {

    static ImageWindow hdrImage = { "HDR Preview", InvalidImageBuffer };
    static ImageWindow blurImage = { "Blur Preview", InvalidImageBuffer };
    static ImageWindow bloomImage = { "Bloom Preview", InvalidImageBuffer };
    static ImageWindow ssaoImage = { "SSAO Preview", InvalidImageBuffer };
    static ImageWindow uiImage = { "UI Preview", InvalidImageBuffer };

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

        if (uiImage.show) {
            uiImage.imageBuffer = ImguiCore::uiPipeline->getRenderTarget();
            uiImage.render();
        }
    }

    void ViewportsMenu::render() {
        if (ImGui::BeginMenu("Viewports")) {
            ImGui::MenuItem(hdrImage.title, null, &hdrImage.show);
            ImGui::MenuItem(blurImage.title, null, &blurImage.show);
            ImGui::MenuItem(bloomImage.title, null, &bloomImage.show);
            ImGui::MenuItem(ssaoImage.title, null, &ssaoImage.show);
            ImGui::MenuItem(uiImage.title, null, &uiImage.show);
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