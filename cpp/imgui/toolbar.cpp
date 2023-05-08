#include <imgui/toolbar.h>

namespace gl {

    static bool showHDRImage = false;
    static bool showBlurImage = false;
    static bool showBloomImage = false;
    static bool showSSAOImage = false;

    static ImageWindow hdrImage = { "HDR Viewport", InvalidImageBuffer };
    static ImageWindow blurImage = { "Blur Viewport", InvalidImageBuffer };
    static ImageWindow bloomImage = { "Bloom Viewport", InvalidImageBuffer };
    static ImageWindow ssaoImage = { "SSAO Viewport", InvalidImageBuffer };

    void Viewports::render() {
        if (showHDRImage) {
            hdrImage.imageBuffer = ImguiCore::hdrRenderer->getRenderTarget();
            hdrImage.render();
        }

        if (showBlurImage) {
            blurImage.imageBuffer = ImguiCore::blurRenderer->getRenderTarget();
            blurImage.render();
        }

        if (showBloomImage) {
            bloomImage.imageBuffer = ImguiCore::bloomRenderer->getRenderTarget();
            bloomImage.render();
        }

        if (showSSAOImage) {
            ssaoImage.imageBuffer = ImguiCore::ssaoRenderer->getRenderTarget();
            ssaoImage.render();
        }
    }

    void ViewportsMenu::render() {
        if (ImGui::BeginMenu("Viewports")) {
            ImGui::MenuItem("HDR", null, &showHDRImage);
            ImGui::MenuItem("Blur", null, &showBlurImage);
            ImGui::MenuItem("Bloom", null, &showBloomImage);
            ImGui::MenuItem("SSAO", null, &showSSAOImage);
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

        ImGui::Begin("Dockspace", &open, windowFlags);

        ImguiCore::dockspaceId = ImGui::GetID("DockSpace");
        ImGui::DockSpace(ImguiCore::dockspaceId, ImVec2(0.0f, 0.0f), ImguiCore::dockspaceFlags);

        if (ImGui::BeginMenuBar()) {

            if (ImGui::BeginMenu("Main")) {
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