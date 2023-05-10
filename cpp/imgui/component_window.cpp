#include <imgui/component_window.h>

#include <features/light.h>

#include <debugging/visuals.h>

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
            auto& tag = sEntity.getComponent<Tag>()->buffer;

            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strncpy_s(buffer, sizeof(buffer), tag.c_str(), sizeof(buffer));
            if (ImGui::InputText("##Tag", buffer, sizeof(buffer))) {
                tag = std::string(buffer);
            }
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

            ImGui::EndPopup();
        }

        ImGui::PopItemWidth();

        ImguiCore::DrawComponent<Transform>("Transform", sEntity, [](Transform& component)
        {
            ImguiCore::DrawTransform(component);
        });

        renderLightComponents();

        renderVisualComponents();
    }

    void ComponentWindow::renderLightComponents() {
        ImguiCore::DrawComponent<DirectLightComponent>("DirectLight", sEntity, [](DirectLightComponent& component)
        {
            ImguiCore::DrawVec4Control("Position", component.value.position);
            ImguiCore::DrawVec4Control("Direction", component.direction);
            ImGui::ColorPicker4("##direct_light_color", glm::value_ptr(component.value.color));
        });

        ImguiCore::DrawComponent<PointLightComponent>("PointLight", sEntity, [](PointLightComponent& component)
        {
            ImguiCore::DrawVec4Control("Position", component.value.position);
            ImguiCore::InputFloat("Constant", component.value.constant, 0.1f);
            ImguiCore::InputFloat("Linear", component.value.linear, 0.1f);
            ImguiCore::InputFloat("Quadratic", component.value.quadratic, 0.1f);
            ImGui::ColorPicker4("##point_light_color", glm::value_ptr(component.value.color));
        });

        ImguiCore::DrawComponent<SpotLightComponent>("SpotLight", sEntity, [](SpotLightComponent& component)
        {
            ImguiCore::DrawVec4Control("Position", component.value.position);
            ImguiCore::DrawVec4Control("Direction", component.value.direction);
            ImguiCore::InputFloat("CutOff", component.value.cutoff, 0.1f);
            component.value.setCutOff(component.value.cutoff);
            ImguiCore::InputFloat("Outer CutOff", component.value.outer, 0.1f);
            component.value.setOuter(component.value.outer);
            ImguiCore::InputFloat("Refraction", component.value.refraction, 0.1f);
            ImGui::ColorPicker4("##spot_light_color", glm::value_ptr(component.value.color));
        });
    }

    void ComponentWindow::renderVisualComponents() {
        ImguiCore::DrawComponent<PolygonVisual>("PolygonVisual", sEntity, [](PolygonVisual& component)
        {
            ImGui::ColorPicker3("##polygon_color", glm::value_ptr(component.color));
            ImguiCore::InputFloat("Thickness", component.thickness, 0.0001f);
        });

        ImguiCore::DrawComponent<NormalVisual>("NormalVisual", sEntity, [](NormalVisual& component)
        {
            ImGui::ColorPicker3("##normal_color", glm::value_ptr(component.color));
            ImguiCore::InputFloat("Length", component.length, 0.1f);
        });

        ImguiCore::DrawComponent<LightVisual>("LightVisual", sEntity, [](LightVisual& component)
        {
            ImGui::ColorPicker4("##light_visual_color", glm::value_ptr(component.color));
            ImguiCore::DrawTransform(component.transform);
        });
    }

}