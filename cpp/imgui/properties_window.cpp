#include <imgui/properties_window.h>

namespace gl {

    static const char* title = "Properties";
    static glm::vec2 resolution = {256, 256 };
    static glm::vec2 position = {0, 0 };
    static bool pInitialized = false;

    static std::unordered_map<KEY, const char*> keyMap = {
            { KEY::W, "W" },
            { KEY::A, "A" },
            { KEY::S, "S" },
            { KEY::D, "D" },

            { KEY::Up, "UP ARROW" },
            { KEY::Left, "LEFT ARROW" },
            { KEY::Down, "DOWN ARROW" },
            { KEY::Right, "RIGHT ARROW" },
    };

    static KEY keys[] = {
            KEY::W, KEY::A, KEY::S, KEY::D,
            KEY::Up, KEY::Left, KEY::Down, KEY::Right,
    };

    static const char* keyLabels[] = {
            "W", "A", "S", "D",
            "UP", "LEFT", "DOWN", "RIGHT"
    };

    void PropertiesWindow::render() {
        if (!pInitialized) {
            ImGui::SetNextWindowDockID(ImguiCore::dockspaceId);
        }

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

            if (ImGui::BeginTabItem("Screen")) {
                auto& screenParams = ImguiCore::screenRenderer->getParams();

                if (ImguiCore::InputFloat("Gamma", screenParams.gamma.value, 0.1f)) {
                    ImguiCore::screenRenderer->updateGamma();
                }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Camera")) {
                Camera* camera = ImguiCore::camera;

                ImguiCore::InputFloat("Horizontal Sens", camera->horizontalSensitivity, 0.01f);
                ImguiCore::InputFloat("Vertical Sens", camera->verticalSensitivity, 0.01f);
                ImguiCore::Checkbox("Enable Look", camera->enableLook);
                ImguiCore::Checkbox("Enable Zoom", camera->enableZoom);
                ImguiCore::Checkbox("Enable Move", camera->enableMove);
                ImguiCore::InputFloat("Move Speed", camera->moveSpeed, 0.1f);

                ImGui::Separator();

                if (ImguiCore::InputFloat("Far Z", camera->zFar, 1.0f)) {
                    camera->updatePerspective();
                }

                if (ImguiCore::InputFloat("Near Z", camera->zNear, 0.1f)) {
                    camera->updatePerspective();
                }

                if (ImguiCore::InputFloat("FOV", camera->fov, 1.0f)) {
                    camera->updatePerspective();
                }

                ImguiCore::InputFloat("Max FOV", camera->maxFov, 1.0f);

                ImGui::Separator();

                if (ImguiCore::InputFloat("Pitch", camera->pitch, 1.0f)) {
                    camera->updateView();
                }

                ImguiCore::InputFloat("Max Pitch", camera->maxPitch, 1.0f);

                if (ImguiCore::InputFloat("Yaw", camera->yaw, 1.0f)) {
                    camera->updateView();
                }

                if (ImguiCore::InputFloat("Roll", camera->roll, 1.0f)) {
                    camera->updateView();
                }
            }

            if (ImGui::BeginTabItem("PostFX")) {

                // HDR
                {
                    auto& hdrParams = ImguiCore::hdrRenderer->getParams();

                    ImguiCore::Checkbox("Enable HDR", ImguiCore::hdrRenderer->isEnabled);

                    if (ImguiCore::InputFloat("Exposure", hdrParams.exposure.value, 0.1f)) {
                        ImguiCore::hdrRenderer->updateExposure();
                    }

                    if (ImguiCore::InputFloat("Shiny", hdrParams.shinyStrength.value, 0.1f)) {
                        ImguiCore::hdrRenderer->updateShinyStrength();
                    }
                }
                ImGui::Separator();

                // Blur
                {
                    auto& blurParams = ImguiCore::blurRenderer->getParams();

                    ImguiCore::Checkbox("Enable Blur", ImguiCore::blurRenderer->isEnabled);

                    if (ImguiCore::InputFloat("Offset", blurParams.offset.value, 0.001f)) {
                        ImguiCore::blurRenderer->updateOffset();
                    }
                }
                ImGui::Separator();

                // Bloom
                {
                    ImguiCore::Checkbox("Enable Bloom", ImguiCore::bloomRenderer->isEnabled);
                    ImguiCore::InputFloat("Strength", ImguiCore::bloomRenderer->getBloomStrength(), 0.01f);
                    ImguiCore::InputFloat("Filter Radius", ImguiCore::bloomRenderer->getFilterRadius(), 0.001f);
                }
                ImGui::Separator();

                // SSAO
                {
                    auto& ssaoParams = ImguiCore::ssaoRenderer->getParams();
                    ImguiCore::Checkbox("Enable SSAO", ImguiCore::ssaoRenderer->isEnabled);
                    ImguiCore::InputInt("Noise Size", ssaoParams.noiseSize.value, 1.0f);
                    ImguiCore::InputInt("Samples Size", ssaoParams.samplesSize.value, 1.0f);
                    ImguiCore::InputFloat("Sample Radius", ssaoParams.sampleRadius.value, 0.1f);
                    ImguiCore::InputFloat("Sample Bias", ssaoParams.sampleBias.value, 0.001f);
                    ImguiCore::InputInt("Occlusion Power", ssaoParams.occlusionPower.value, 1.0f);
                }

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();
    }

}