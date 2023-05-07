#include <imgui/screen_properties.h>

namespace gl {

    const char* ScreenProperties::title = "Screen Properties";
    glm::vec2 ScreenProperties::resolution = { 256, 256 };
    glm::vec2 ScreenProperties::position = { 0, 0 };

    ScreenRenderer* ScreenProperties::screenRenderer = null;
    HdrRenderer* ScreenProperties::hdrRenderer = null;
    BlurRenderer* ScreenProperties::blurRenderer = null;
    BloomRenderer* ScreenProperties::bloomRenderer = null;
    SsaoRenderer* ScreenProperties::ssaoRenderer = null;

    ImageWindow ScreenProperties::screenImage = { "Screen Viewport", InvalidImageBuffer };
    ImageWindow ScreenProperties::hdrImage = { "HDR Viewport", InvalidImageBuffer };
    ImageWindow ScreenProperties::blurImage = { "Blur Viewport", InvalidImageBuffer };
    ImageWindow ScreenProperties::bloomImage = { "Bloom Viewport", InvalidImageBuffer };
    ImageWindow ScreenProperties::ssaoImage = { "SSAO Viewport", InvalidImageBuffer };

    bool pInitialized = false;

    bool showScreenImage = false;
    bool showHDRImage = false;
    bool showBlurImage = false;
    bool showBloomImage = false;
    bool showSSAOImage = false;

    void ScreenProperties::render() {
        static bool open = true;
        if (!ImGui::Begin(title, &open)) {
            ImGui::End();
            return;
        }

        if (!pInitialized) {
            pInitialized = true;
            ImGui::SetWindowPos({ position.x, position.y });
            ImGui::SetWindowSize({ resolution.x, resolution.y });
        }

        ImGui::Checkbox("Viewport", &showScreenImage);
        auto& screenParams = screenRenderer->getParams();
        if (ImGui::DragFloat("Gamma", &screenParams.gamma.value, 0.1f, 1.0f, 5.0f)) {
            screenRenderer->updateGamma();
        }

        ImGui::Checkbox("HDR", &hdrRenderer->isEnabled);
        if (hdrRenderer->isEnabled) {
            auto& hdrParams = hdrRenderer->getParams();
            if (ImGui::CollapsingHeader("HDR")) {
                ImGui::Checkbox("Viewport", &showHDRImage);
                if (ImGui::InputFloat("Exposure", &hdrParams.exposure.value)) {
                    hdrRenderer->updateExposure();
                }
                if (ImGui::InputFloat("Shiny", &hdrParams.shinyStrength.value)) {
                    hdrRenderer->updateShinyStrength();
                }
            }
        }

        ImGui::Checkbox("Blur", &blurRenderer->isEnabled);
        if (blurRenderer->isEnabled) {
            auto& blurParams = blurRenderer->getParams();
            if (ImGui::CollapsingHeader("Blur")) {
                ImGui::Checkbox("Viewport", &showBlurImage);
                if (ImGui::InputFloat("Offset", &blurParams.offset.value)) {
                    blurRenderer->updateOffset();
                }
            }
        }

        ImGui::Checkbox("Bloom", &bloomRenderer->isEnabled);
        if (bloomRenderer->isEnabled) {
            if (ImGui::CollapsingHeader("Bloom")) {
                ImGui::Checkbox("Viewport", &showBloomImage);
                ImGui::InputFloat("Strength", &bloomRenderer->getBloomStrength());
                ImGui::InputFloat("FilterRadius", &bloomRenderer->getFilterRadius());
            }
        }

        ImGui::Checkbox("SSAO", &ssaoRenderer->isEnabled);
        if (ssaoRenderer->isEnabled) {
            auto& ssaoParams = ssaoRenderer->getParams();
            if (ImGui::CollapsingHeader("SSAO")) {
                ImGui::Checkbox("Viewport", &showSSAOImage);
                ImGui::InputInt("NoiseSize", &ssaoParams.noiseSize.value, 0, 10);
                ImGui::InputInt("SamplesSize", &ssaoParams.samplesSize.value, 0, 128);
                ImGui::InputFloat("SampleRadius", &ssaoParams.sampleRadius.value, 0.0f, 10.0f, "%.1");
                ImGui::InputFloat("SampleBias", &ssaoParams.sampleBias.value, 0.0f, 0.1f, "%.001");
                ImGui::InputInt("OcclusionPower", &ssaoParams.occlusionPower.value, 0, 10);
            }
        }

        ImGui::End();

        if (showScreenImage) {
            screenImage.imageBuffer = screenRenderer->getParams().sceneBuffer;
            screenImage.render();
        }

        if (showHDRImage) {
            hdrImage.imageBuffer = hdrRenderer->getRenderTarget();
            hdrImage.render();
        }

        if (showBlurImage) {
            blurImage.imageBuffer = blurRenderer->getRenderTarget();
            blurImage.render();
        }

        if (showBloomImage) {
            bloomImage.imageBuffer = bloomRenderer->getRenderTarget();
            bloomImage.render();
        }

        if (showSSAOImage) {
            ssaoImage.imageBuffer = ssaoRenderer->getRenderTarget();
            ssaoImage.render();
        }
    }

}