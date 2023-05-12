#include <imgui/component_window.h>

#include <features/lighting/light.h>

#include <debugging/visuals.h>

#include <text/text.h>

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
        if (sEntity.validComponent<Tag>()) {
            ImguiCore::InputText(sEntity.getComponent<Tag>()->buffer);
        }

        ImGui::SameLine();
        ImGui::PushItemWidth(-1);

        if (ImGui::Button("Add Component"))
            ImGui::OpenPopup("AddComponent");

        if (ImGui::BeginPopup("AddComponent")) {
            displayAddComponent<Transform>("Transform");

            displayAddComponent<PolygonVisual>("PolygonVisual");
            displayAddComponent<NormalVisual>("NormalVisual");
            displayAddComponent<LightVisual>("LightVisual");

            displayAddComponent<DirectLightComponent>("DirectLight");
            displayAddComponent<PointLightComponent>("PointLight");
            displayAddComponent<SpotLightComponent>("SpotLight");

            displayAddComponent<Text2d>("Text2D");
            displayAddComponent<Text3d>("Text3D");

            ImGui::EndPopup();
        }

        ImGui::PopItemWidth();

        ImguiCore::DrawComponent<Transform>("Transform", sEntity, [](Transform& component)
        {
            ImguiCore::DrawTransform(component);
        });

        renderLightComponents();

        renderVisualComponents();

        renderTextComponents();
    }

    void ComponentWindow::renderLightComponents() {
        ImguiCore::DrawComponent<DirectLightComponent>("DirectLight", sEntity, [](DirectLightComponent& component)
        {
            ImguiCore::DrawVec4Control("Position", component.position);
            ImguiCore::DrawVec3Control("Direction", component.direction);
            ImguiCore::DrawColor4Control("Color", component.color);
        });

        ImguiCore::DrawComponent<PointLightComponent>("PointLight", sEntity, [](PointLightComponent& component)
        {
            ImguiCore::DrawVec4Control("Position", component.position);
            ImguiCore::DrawColor4Control("Color", component.color);
            ImguiCore::InputFloat("Constant", component.constant, 0.1f);
            ImguiCore::InputFloat("Linear", component.linear, 0.1f);
            ImguiCore::InputFloat("Quadratic", component.quadratic, 0.1f);
        });

        ImguiCore::DrawComponent<SpotLightComponent>("SpotLight", sEntity, [](SpotLightComponent& component)
        {
            ImguiCore::DrawVec4Control("Position", component.position);
            ImguiCore::DrawVec4Control("Direction", component.direction);
            ImguiCore::DrawColor4Control("Color", component.color);
            ImguiCore::InputFloat("CutOff", component.cutoff, 0.1f);
            component.setCutOff(component.cutoff);
            ImguiCore::InputFloat("Outer CutOff", component.outer, 0.1f);
            component.setOuter(component.outer);
            ImguiCore::InputFloat("Refraction", component.refraction, 0.1f);
        });
    }

    void ComponentWindow::renderVisualComponents() {
        ImguiCore::DrawComponent<PolygonVisual>("PolygonVisual", sEntity, [](PolygonVisual& component)
        {
            ImguiCore::InputFloat("Thickness", component.thickness, 0.0001f);
            ImGui::SeparatorText("Color");
            ImguiCore::DrawColor3Control("##polygon_color", component.color);
        });

        ImguiCore::DrawComponent<NormalVisual>("NormalVisual", sEntity, [](NormalVisual& component)
        {
            ImguiCore::InputFloat("Length", component.length, 0.1f);
            ImGui::SeparatorText("Color");
            ImguiCore::DrawColor3Control("##normal_color", component.color);
        });

        ImguiCore::DrawComponent<LightVisual>("LightVisual", sEntity, [](LightVisual& component)
        {
            ImGui::SeparatorText("Transform");
            ImguiCore::DrawTransform(component.transform);
            ImGui::SeparatorText("Color");
            ImguiCore::DrawColor4Control("##light_visual_color", component.color);
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

}