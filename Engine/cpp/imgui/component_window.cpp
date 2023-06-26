#include <imgui/component_window.h>

#include <core/imgui_core.h>

namespace gl {

    const char* ComponentWindow::title = "Components";
    glm::vec2 ComponentWindow::position = { 0, 0 };
    glm::vec2 ComponentWindow::resolution = { 256, 256 };
    ImGuiWindowFlags ComponentWindow::windowFlags = ImGuiWindowFlags_None;
    Entity ComponentWindow::sEntity;

    static bool pOpen = false;
    static bool pInitialized = false;

    void ComponentWindow::render() {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

        if (!ImGui::Begin(title, &pOpen, windowFlags)) {
            end();
            return;
        }

        if (!pInitialized) {
            pInitialized = true;
            ImGui::SetWindowPos({ position.x, position.y });
            ImGui::SetWindowSize({ resolution.x, resolution.y });
        }

        sEntity = ImguiCore::selectedEntity;
        if (sEntity.valid()) {
            renderComponents();
        }

        end();
    }

    void ComponentWindow::end() {
        ImGui::PopStyleVar(2);
        ImGui::End();
    }

    void ComponentWindow::renderComponents() {
        Tag* tag = sEntity.getComponent<Tag>();
        if (tag) {
            ImguiCore::InputText(tag->buffer);
        }

        ImGui::SameLine();

        if (ImguiCore::IconButton("##add_component", "+"))
            ImGui::OpenPopup("AddComponent");

        if (ImGui::BeginPopup("AddComponent")) {
            displayAddComponent<Transform2d>("Transform2D");
            displayAddComponent<Transform>("Transform");

            displayAddComponent<PhongLightComponent>("PhongLight");
            displayAddComponent<DirectLightComponent>("DirectLight");
            displayAddComponent<PointLightComponent>("PointLight");
            displayAddComponent<SpotLightComponent>("SpotLight");

            displayAddComponent<Text2d>("Text2D");
            displayAddComponent<Text3d>("Text3D");

            displayAddComponent<Outline>("Outline");

            displayAddComponent<Shadowable>("Shadowing");

            ImGui::EndPopup();
        }

        renderTransformComponents();

        renderLightComponents();

        renderTextComponents();

        renderMaterialComponents();

        renderOutlineComponent();

        renderShadowComponents();
    }

    void ComponentWindow::renderTransformComponents() {
        ImguiCore::DrawComponent<Transform2d>("Transform2D", sEntity, [](Transform2d& component) {
            ImguiCore::DrawTransform2d(component);
        });
        ImguiCore::DrawComponent<Transform>("Transform", sEntity, [](Transform& component) {
            ImguiCore::DrawTransform(component);
        });
    }

    void ComponentWindow::renderLightComponents() {
        ImguiCore::DrawComponent<PhongLightComponent>("PhongLight", sEntity, [](PhongLightComponent& component)
        {
            bool positionChanged = ImguiCore::DrawVec4Control("Position", component.position);
            bool colorChanged = ImguiCore::DrawLightColorControl("PhongLightColor", component.color);

            if (positionChanged || colorChanged) {
                LightStorage::update();
            }
        });

        ImguiCore::DrawComponent<DirectLightComponent>("DirectLight", sEntity, [](DirectLightComponent& component)
        {
            bool positionChanged = ImguiCore::DrawVec4Control("Position", component.position);
            bool dirChanged = ImguiCore::DrawVec3Control("Direction", component.direction);
            bool colorChanged = ImguiCore::DrawLightColorControl("DirectLightColor", component.color);

            if (positionChanged || dirChanged || colorChanged) {
                LightStorage::update();
            }
        });

        ImguiCore::DrawComponent<PointLightComponent>("PointLight", sEntity, [](PointLightComponent& component)
        {
            bool positionChanged = ImguiCore::DrawVec4Control("Position", component.position);
            bool colorChanged = ImguiCore::DrawLightColorControl("PointLightColor", component.color);
            bool constantChanged = ImguiCore::InputFloat("Constant", component.constant, 0.1f);
            bool linearChanged = ImguiCore::InputFloat("Linear", component.linear, 0.1f);
            bool quadraticChanged = ImguiCore::InputFloat("Quadratic", component.quadratic, 0.1f);

            if (positionChanged || colorChanged || constantChanged || linearChanged || quadraticChanged) {
                LightStorage::update();
            }
        });

        ImguiCore::DrawComponent<SpotLightComponent>("SpotLight", sEntity, [](SpotLightComponent& component)
        {
            bool positionChanged = ImguiCore::DrawVec4Control("Position", component.position);
            bool directionChanged = ImguiCore::DrawVec4Control("Direction", component.direction);
            bool colorChanged = ImguiCore::DrawLightColorControl("SpotLightColor", component.color);
            bool cutoffChanged = ImguiCore::InputFloat("CutOff", component.cutoff, 0.1f);
            component.setCutOff(component.cutoff);
            bool outerChanged = ImguiCore::InputFloat("Outer CutOff", component.outer, 0.1f);
            component.setOuter(component.outer);

            if (positionChanged || directionChanged || colorChanged || cutoffChanged || outerChanged) {
                LightStorage::update();
            }
        });
    }

    void ComponentWindow::renderTextComponents() {
        ImguiCore::DrawComponent<Text2d>("Text2D", sEntity, [](Text2d& component) {
            ImGui::SeparatorText("Input");
            if (ImguiCore::InputText(component.buffer)) {
                component.update();
            }
            ImGui::SeparatorText("Font Style");
            ImguiCore::DrawFontStyle(component.style);
            ImGui::SeparatorText("Transform 2D");
            ImguiCore::DrawTransform2d(component.transform);
        });

        ImguiCore::DrawComponent<Text3d>("Text3D", sEntity, [](Text3d& component) {
            ImGui::SeparatorText("Input");
            if (ImguiCore::InputText(component.buffer)) {
                component.update();
            }
            ImGui::SeparatorText("Font Style");
            ImguiCore::DrawFontStyle(component.style);
            ImGui::SeparatorText("Transform");
            ImguiCore::DrawTransform(component.transform);
        });
    }

    void ComponentWindow::renderMaterialComponents() {
        ImguiCore::DrawComponent<Material>("Material", sEntity, [](Material& component) {
            if (ImGui::CollapsingHeader("Base Color")) {
                ImGui::PushID("##base_color");
                ImguiCore::ColorEdit4("Color", component.color);
                ImGui::PopID();
                ImguiCore::Checkbox("Albedo Mapping", component.enableAlbedo);

                bool transparent = sEntity.validComponent<Transparent>();
                ImguiCore::Checkbox("Transparent", transparent);

                if (sEntity.invalidComponent<Transparent>() && transparent) {
                    sEntity.addComponent<Transparent>();
                    sEntity.removeComponent<Opaque>();
                }

                else if (sEntity.validComponent<Transparent>() && !transparent) {
                    sEntity.removeComponent<Transparent>();
                    sEntity.addComponent<Opaque>();
                }
            }

            if (ImGui::CollapsingHeader("Bumping")) {
                ImguiCore::Checkbox("Normal Mapping", component.enableNormal);
            }

            if (ImGui::CollapsingHeader("Parallax Occlusion")) {
                ImguiCore::Checkbox("Parallax Mapping", component.enableParallax);
                ImGui::SliderFloat("Height Scale", &component.heightScale, 0.0f, 2.0f);
                ImGui::SliderFloat("Min Layers", &component.parallaxMinLayers, 0, 8);
                ImGui::SliderFloat("Max Layers", &component.parallaxMaxLayers, 8, 32);
            }

            if (ImGui::CollapsingHeader("Metallic")) {
                ImguiCore::Checkbox("Metallic Mapping", component.enableMetallic);
                ImGui::PushID("##metallic_factor");
                ImGui::SliderFloat("Intensity", &component.metallicFactor, 0.0f, 1.0f);
                ImGui::PopID();
            }

            if (ImGui::CollapsingHeader("Roughness")) {
                ImguiCore::Checkbox("Roughness Mapping", component.enableRoughness);
                ImGui::PushID("##roughness_factor");
                ImGui::SliderFloat("Intensity", &component.roughnessFactor, 0.0f, 1.0f);
                ImGui::PopID();
            }

            if (ImGui::CollapsingHeader("Ambient Occlusion")) {
                ImguiCore::Checkbox("AO Mapping", component.enableAO);
                ImGui::PushID("##ao_factor");
                ImGui::SliderFloat("Intensity", &component.aoFactor, 0.0f, 1.0f);
                ImGui::PopID();
            }

            if (ImGui::CollapsingHeader("Emission")) {
                ImguiCore::Checkbox("Emission Mapping", component.enableEmission);
                ImGui::PushID("##emission_factor");
                ImguiCore::DrawColor3Control("Intensity", component.emissionFactor);
                ImGui::PopID();
            }
        });
    }

    void ComponentWindow::renderOutlineComponent() {
        ImguiCore::DrawComponent<Outline>("Outline", sEntity, [](Outline& component) {
            ImGui::PushID("##outline_color");
            ImguiCore::ColorEdit4("Color", component.color);
            ImGui::PopID();

            ImGui::PushID("##outline_thickness");
            ImGui::SliderFloat("Thickness", &component.thickness, 0.0f, 1.0f);
            ImGui::PopID();
        });
    }

    void ComponentWindow::renderShadowComponents() {
        ImguiCore::DrawComponent<Shadowable>("Shadowing", sEntity, [](Shadowable& component) {});
    }

}