#include <imgui/toolbar.h>

#include <core/imgui_core.h>

#include <imgui/image_window.h>

namespace gl {

    static ImageWindow positionsImage = { "Positions", InvalidImageBuffer };
    static ImageWindow normalsImage = { "Normals", InvalidImageBuffer };
    static ImageWindow albedosImage = { "Albedos", InvalidImageBuffer };
    static ImageWindow pbrParamsImage = { "PBR", InvalidImageBuffer };
    static ImageWindow emissionImage = { "Emission", InvalidImageBuffer };

    static ImageWindow pcfDirectShadowImage = { "PCF Direct Shadow", InvalidImageBuffer };

    static ImageWindow hdrImage = { "HDR", InvalidImageBuffer };
    static ImageWindow blurImage = { "Blur", InvalidImageBuffer };
    static ImageWindow bloomImage = { "Bloom", InvalidImageBuffer };
    static ImageWindow ssaoImage = { "SSAO", InvalidImageBuffer };
    static ImageWindow fxaaImage = { "FXAA", InvalidImageBuffer };

    static ImageWindow uiImage = { "UI", InvalidImageBuffer };

    void Viewports::render() {
        if (positionsImage.show) {
            positionsImage.imageBuffer = ImguiCore::pbrPipeline->getGBuffer().position;
            positionsImage.render();
        }

        if (normalsImage.show) {
            normalsImage.imageBuffer = ImguiCore::pbrPipeline->getGBuffer().normal;
            normalsImage.render();
        }

        if (albedosImage.show) {
            albedosImage.imageBuffer = ImguiCore::pbrPipeline->getGBuffer().albedo;
            albedosImage.render();
        }

        if (pbrParamsImage.show) {
            pbrParamsImage.imageBuffer = ImguiCore::pbrPipeline->getGBuffer().pbrParams;
            pbrParamsImage.render();
        }

        if (emissionImage.show) {
            emissionImage.imageBuffer = ImguiCore::pbrPipeline->getGBuffer().emission;
            emissionImage.render();
        }

        if (pcfDirectShadowImage.show) {
            pcfDirectShadowImage.imageBuffer = ImguiCore::shadowPipeline->directShadow.map.buffer;
            pcfDirectShadowImage.render();
        }

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

        if (fxaaImage.show) {
            fxaaImage.imageBuffer = ImguiCore::fxaaRenderer->getRenderTarget();
            fxaaImage.render();
        }

        if (uiImage.show) {
            uiImage.imageBuffer = ImguiCore::uiPipeline->getRenderTarget();
            uiImage.render();
        }
    }

    void ViewportsMenu::render() {
        if (ImGui::BeginMenu("Viewports")) {
            ImGui::MenuItem(positionsImage.title, null, &positionsImage.show);
            ImGui::MenuItem(normalsImage.title, null, &normalsImage.show);
            ImGui::MenuItem(albedosImage.title, null, &albedosImage.show);
            ImGui::MenuItem(pbrParamsImage.title, null, &pbrParamsImage.show);
            ImGui::MenuItem(emissionImage.title, null, &emissionImage.show);

            ImGui::MenuItem(pcfDirectShadowImage.title, null, &pcfDirectShadowImage.show);

            ImGui::MenuItem(hdrImage.title, null, &hdrImage.show);
            ImGui::MenuItem(blurImage.title, null, &blurImage.show);
            ImGui::MenuItem(bloomImage.title, null, &bloomImage.show);
            ImGui::MenuItem(ssaoImage.title, null, &ssaoImage.show);
            ImGui::MenuItem(fxaaImage.title, null, &fxaaImage.show);

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