#include <imgui/gizmo.h>

#include <core/imgui_core.h>

namespace gl {

    bool Gizmo::enableTranslation = false;
    bool Gizmo::enableRotation = false;
    bool Gizmo::enableScale = false;
    bool Gizmo::enableWorldMode = false;

    void Gizmo::render() {
        ImGuiIO& io = *ImguiCore::IO;
        Scene* scene = ImguiCore::scene;
        auto& camera = *ImguiCore::camera;

        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();

        float width = ImGui::GetWindowWidth();
        float height = ImGui::GetWindowHeight();
        ImVec2 position = ImGui::GetWindowPos();
        ImGuizmo::SetRect(position.x, position.y, width, height);

        const float* view = glm::value_ptr(camera.view());
        const float* perspective = glm::value_ptr(camera.perspective());

        ImGuizmo::MODE mode = enableWorldMode ? ImGuizmo::WORLD : ImGuizmo::LOCAL;

        // draw gizmo for scene objects
        scene->eachComponent<GizmoTransformComponent>([=](GizmoTransformComponent* component) {
            auto& transform = *scene->getComponent<Transform>(component->entityId);

            if (enableTranslation) {
                ImGuizmo::Manipulate(
                        view,
                        perspective,
                        ImGuizmo::TRANSLATE,
                        mode,
                        glm::value_ptr(transform.value)
                );
            }

            if (enableRotation) {
                ImGuizmo::Manipulate(
                        view,
                        perspective,
                        ImGuizmo::ROTATE,
                        mode,
                        glm::value_ptr(transform.value)
                );
            }

            if (enableScale) {
                ImGuizmo::Manipulate(
                        view,
                        perspective,
                        ImGuizmo::SCALE,
                        mode,
                        glm::value_ptr(transform.value)
                );
            }
        });

        // draw gizmo for lights

        scene->eachComponent<GizmoPhongLight>([=](GizmoPhongLight* component) {
            auto* phongLight = scene->getComponent<PhongLightComponent>(component->entityId);

            if (enableTranslation && phongLight) {
                glm::mat4 translate(1.0f);
                translate = glm::translate(translate, glm::vec3(phongLight->position));
                ImGuizmo::Manipulate(
                        view,
                        perspective,
                        ImGuizmo::TRANSLATE,
                        mode,
                        glm::value_ptr(translate)
                );
                phongLight->position = glm::vec4(translate[3]);
            }
        });

        scene->eachComponent<GizmoDirectLight>([=](GizmoDirectLight* component) {
            auto* directLight = scene->getComponent<DirectLightComponent>(component->entityId);

            if (enableTranslation && directLight) {
                glm::mat4 translate(1.0f);
                translate = glm::translate(translate, glm::vec3(directLight->position));
                ImGuizmo::Manipulate(
                        view,
                        perspective,
                        ImGuizmo::TRANSLATE,
                        mode,
                        glm::value_ptr(translate)
                );
                directLight->position = glm::vec4(translate[3]);
            }
        });

        scene->eachComponent<GizmoPointLight>([=](GizmoPointLight* component) {
            auto* pointLight = scene->getComponent<PointLightComponent>(component->entityId);

            if (enableTranslation && pointLight) {
                glm::mat4 translate(1.0f);
                translate = glm::translate(translate, glm::vec3(pointLight->position));
                ImGuizmo::Manipulate(
                        view,
                        perspective,
                        ImGuizmo::TRANSLATE,
                        mode,
                        glm::value_ptr(translate)
                );
                pointLight->position = glm::vec4(translate[3]);
            }
        });

        scene->eachComponent<GizmoSpotLight>([=](GizmoSpotLight* component) {
            auto* spotLight = scene->getComponent<SpotLightComponent>(component->entityId);

            if (enableTranslation && spotLight) {
                glm::mat4 translate(1.0f);
                translate = glm::translate(translate, glm::vec3(spotLight->position));
                ImGuizmo::Manipulate(
                        view,
                        perspective,
                        ImGuizmo::TRANSLATE,
                        mode,
                        glm::value_ptr(translate)
                );
                spotLight->position = glm::vec4(translate[3]);
            }
        });
    }

}