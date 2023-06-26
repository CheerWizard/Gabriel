#include <core/imgui_core.h>
#include <imgui/properties_window.h>

namespace gl {

    static const char* title = "Properties";
    static glm::vec2 resolution = { 256, 256 };
    static glm::vec2 position = { 0, 0 };
    static bool pInitialized = false;

    static KEY keys[] = {
            KEY::W, KEY::A, KEY::S, KEY::D,
            KEY::Up, KEY::Left, KEY::Down, KEY::Right,
    };
    static const char* keyLabels[] = {
            "W", "A", "S", "D",
            "UP", "LEFT", "DOWN", "RIGHT"
    };

    void PropertiesWindow::render() {
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

        if (ImGui::BeginTabBar(title)) {

            if (ImGui::BeginTabItem("Display")) {
                Window* window = ImguiCore::window;

                ImGui::Text("%s", Timer::getCurrentDateTime().c_str());
                ImGui::Separator();

                ImGui::Text("%dhz", window->getRefreshRate());
                ImGui::Text("%ffps", Timer::getFPS());
                ImGui::Text("%fms", Timer::getDeltaMillis());
                ImGui::Separator();

                bool fullscreen = window->getFullscreen();
                if (ImguiCore::Checkbox("Fullscreen", fullscreen)) {
                    window->toggleWindowMode();
                }

                bool vsync = window->getVSync();
                if (ImguiCore::Checkbox("VSync", vsync)) {
                    window->toggleVSync();
                }

                auto& screenParams = ImguiCore::screenRenderer->getParams();
                if (ImguiCore::SliderFloat("Gamma", screenParams.gamma.value, 0, 5)) {
                    ImguiCore::screenRenderer->updateGamma();
                }

                if (ImGui::CollapsingHeader("Anti-Aliasing")) {
                    auto* fxaaRenderer = ImguiCore::fxaaRenderer;
                    if (fxaaRenderer) {
                        ImGui::SeparatorText("FXAA");

                        ImGui::PushID("##fxaa_enable");
                        ImguiCore::Checkbox("Enable", fxaaRenderer->isEnabled);
                        ImGui::PopID();

                        ImguiCore::SliderFloat("Span Max", fxaaRenderer->getParams().spanMax.value, 0, 16);
                        ImguiCore::SliderFloat("Reduce Min", fxaaRenderer->getParams().reduceMin.value, 0, 1);
                        ImguiCore::SliderFloat("Reduce Mul", fxaaRenderer->getParams().reduceMul.value, 0, 1);
                    }
                }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Control")) {

                if (ImGui::CollapsingHeader("Camera")) {
                    Camera* camera = ImguiCore::camera;

                    ImGui::SeparatorText("World Space");
                    ImguiCore::DrawVec3Control("Position", camera->position);
                    ImguiCore::DrawVec3Control("Front", camera->front);
                    ImguiCore::DrawVec3Control("Up", camera->up);

                    ImGui::SeparatorText("Navigation");
                    ImguiCore::InputFloat("Horizontal Sens", camera->horizontalSensitivity, 0.01f);
                    ImguiCore::InputFloat("Vertical Sens", camera->verticalSensitivity, 0.01f);
                    ImguiCore::Checkbox("Enable Look", camera->enableLook);
                    ImguiCore::Checkbox("Enable Zoom", camera->enableZoom);
                    ImguiCore::Checkbox("Enable Move", camera->enableMove);
                    ImguiCore::InputFloat("Move Speed", camera->moveSpeed, 0.1f);

                    ImGui::SeparatorText("Perspective Space");
                    if (ImguiCore::InputFloat("Far Z", camera->zFar, 1.0f)) {
                        camera->updatePerspective();
                    }
                    if (ImguiCore::InputFloat("Near Z", camera->zNear, 0.1f)) {
                        camera->updatePerspective();
                    }
                    if (ImguiCore::InputFloat("FOV", camera->fov, 1.0f)) {
                        camera->updatePerspective();
                    }

                    ImGui::SeparatorText("View Space");
                    if (ImguiCore::InputFloat("Pitch", camera->pitch, 1.0f)) {
                        camera->updateView();
                    }
                    if (ImguiCore::InputFloat("Yaw", camera->yaw, 1.0f)) {
                        camera->updateView();
                    }
                    if (ImguiCore::InputFloat("Roll", camera->roll, 1.0f)) {
                        camera->updateView();
                    }
                }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("PostFX")) {

                if (ImGui::CollapsingHeader("HDR")) {
                    auto& hdrParams = ImguiCore::hdrRenderer->getParams();

                    ImguiCore::Checkbox("Enable HDR", ImguiCore::hdrRenderer->isEnabled);

                    if (ImguiCore::SliderFloat("Exposure", hdrParams.exposure.value, 0, 10)) {
                        ImguiCore::hdrRenderer->updateExposure();
                    }

                    if (ImguiCore::SliderFloat("Shiny", hdrParams.shinyStrength.value, 0, 16)) {
                        ImguiCore::hdrRenderer->updateShinyStrength();
                    }
                }

                if (ImGui::CollapsingHeader("Blur")) {
                    auto& blurParams = ImguiCore::blurRenderer->getParams();

                    ImguiCore::Checkbox("Enable Blur", ImguiCore::blurRenderer->isEnabled);

                    if (ImguiCore::InputFloat("Offset", blurParams.offset.value, 0.001f)) {
                        ImguiCore::blurRenderer->updateOffset();
                    }
                }

                if (ImGui::CollapsingHeader("Bloom")) {
                    ImguiCore::Checkbox("Enable Bloom", ImguiCore::bloomRenderer->isEnabled);
                    ImguiCore::SliderFloat("Strength", ImguiCore::bloomRenderer->getBloomStrength(), 0.0f, 1.0f);
                    ImguiCore::SliderFloat("Filter Radius", ImguiCore::bloomRenderer->getFilterRadius(), 0.0f, 1.0f);
                }

                if (ImGui::CollapsingHeader("SSAO")) {
                    auto& ssaoParams = ImguiCore::ssaoRenderer->getParams();
                    ImguiCore::Checkbox("Enable SSAO", ImguiCore::ssaoRenderer->isEnabled);
                    ImguiCore::InputInt("Noise Size", ssaoParams.noiseSize.value);
                    ImguiCore::InputInt("Samples Size", ssaoParams.samplesSize.value);
                    ImguiCore::InputFloat("Sample Radius", ssaoParams.sampleRadius.value, 0.1f);
                    ImguiCore::InputFloat("Sample Bias", ssaoParams.sampleBias.value, 0.001f);
                    ImguiCore::SliderInt("Occlusion Power", ssaoParams.occlusionPower.value, 0, 10);
                }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Global")) {

                if (ImGui::CollapsingHeader("Environment")) {
                }

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();
    }

}