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

    static ImageWindow accumulationImage = { "Accumulation", InvalidImageBuffer };
    static ImageWindow revealImage = {"Reveal", InvalidImageBuffer };
    static ImageWindow transparentImage = { "Transparent", InvalidImageBuffer };

    static ImageWindow uiImage = { "UI", InvalidImageBuffer };

    static ImageWindow visualsImage = { "Visuals", InvalidImageBuffer };

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

        if (visualsImage.show) {
            visualsImage.imageBuffer = ImguiCore::visualsPipeline->getRenderTarget();
            visualsImage.render();
        }

        if (accumulationImage.show) {
            accumulationImage.imageBuffer = ImguiCore::transparentRenderer->getParams().accumBuffer;
            accumulationImage.render();
        }

        if (revealImage.show) {
            revealImage.imageBuffer = ImguiCore::transparentRenderer->getParams().revealBuffer;
            revealImage.render();
        }

        if (transparentImage.show) {
            transparentImage.imageBuffer = ImguiCore::transparentRenderer->getRenderTarget();
            transparentImage.render();
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
            ImGui::MenuItem(visualsImage.title, null, &visualsImage.show);

            ImGui::MenuItem(accumulationImage.title, null, &accumulationImage.show);
            ImGui::MenuItem(revealImage.title, null, &revealImage.show);
            ImGui::MenuItem(transparentImage.title, null, &transparentImage.show);

            ImGui::EndMenu();
        }
    }

    bool Toolbar::sInitialized = false;
    int Toolbar::sX = 0;
    int Toolbar::sY = 0;
    int Toolbar::sWidth = 800;
    int Toolbar::sHeight = 600;

    void Toolbar::render() {
        static bool show = true;

        if (!sInitialized || ImguiCore::frameBufferResized) {
            sInitialized = true;
            ImguiCore::frameBufferResized = false;
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
        }

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        windowFlags |= ImGuiWindowFlags_NoTitleBar;
        windowFlags |= ImGuiWindowFlags_NoCollapse;
        windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        windowFlags |= ImGuiWindowFlags_NoBackground;

        ImGui::Begin("DockSpace", &show, windowFlags);
        ImGuiID dsId = ImGui::GetID("Dashboard");
        ImguiCore::dockspaceId = dsId;
        ImGui::DockSpace(dsId, { 0, 0 }, ImguiCore::dockspaceFlags);

        ImGui::SetNextWindowSize({ ImGui::GetWindowWidth(), 64 });

        if (ImGui::BeginMenuBar()) {

            if (ImguiCore::logo.buffer.id != InvalidImageBuffer) {
                const auto id = reinterpret_cast<ImTextureID>((unsigned long long) ImguiCore::logo.buffer.id);
                ImVec2 size = {
                        static_cast<float>(ImguiCore::logo.image.width),
                        static_cast<float>(ImguiCore::logo.image.height)
                };
                ImGui::Image(id, size, { 0, 1 }, { 1, 0 });
            }

            if (ImGui::BeginMenu("File")) {

                if (ImGui::MenuItem("New", "Ctrl+N")) {

                }

                if (ImGui::MenuItem("Open", "Ctrl+O")) {

                }

                if (ImGui::MenuItem("Save", "Ctrl+S")) {

                }

                ImGui::Separator();

                if (ImGui::MenuItem("Settings", "Alt+S")) {

                }

                ImGui::Separator();

                if (ImGui::MenuItem("Exit", "Esc", false)) {
                    ImguiCore::close = true;
                }

                ImGui::EndMenuBar();
            }

            ViewportsMenu::render();

            ImguiCore::Spacing(ImGui::GetWindowWidth() - 410, 1);

            if (ImguiCore::IconButton("##minimize_button", "_", { 72, 24 }, 0)) {
                ImguiCore::window->minimize();
            }

            ImGui::SameLine(0, 1);

            static bool fullscreen = false;
            if (ImguiCore::IconButton("##resize_button", ICON_CI_SCREEN_FULL, { 72, 24 }, 0)) {
                fullscreen = !fullscreen;
                if (fullscreen) {
                    ImguiCore::window->setFullScreen();
                } else {
                    ImguiCore::window->setWindowed();
                }
            }

            ImGui::SameLine(0, 1);

            if (ImguiCore::IconButton("##close_button", ICON_CI_CLOSE, { 72, 24 }, 0)) {
                ImguiCore::close = true;
            }

            ImGui::EndMenuBar();
        }

        ImVec2 pos = ImGui::GetWindowPos();
        int width = static_cast<int>(ImGui::GetWindowWidth());
        int height = static_cast<int>(ImGui::GetWindowWidth());
        int x = static_cast<int>(pos.x);
        int y = static_cast<int>(pos.y);

        if (x != sX || y != sY) {
            sX = x;
            sY = y;
            ImguiCore::window->move(x, y);
        }

        if (width != sWidth || height != sHeight) {
            sWidth = width;
            sHeight = height;
            ImguiCore::window->resize(width, height);
        }

        end();

        Viewports::render();
    }

    void Toolbar::end() {
        ImGui::PopStyleVar(3);
        ImGui::End();
    }

}