#include <imgui/gizmo.h>

#include <core/imgui_core.h>

namespace gl {

    bool Gizmo::enableTranslation = false;
    bool Gizmo::enableRotation = false;
    bool Gizmo::enableScale = false;
    bool Gizmo::enableWorldMode = false;

    Entity Gizmo::sEntity;

    glm::mat4 Gizmo::sView;
    glm::mat4 Gizmo::sPerspective;

    static ImGuizmo::MODE pMode;

    void Gizmo::render(const Entity& entity) {
        ImGuiIO& io = *ImguiCore::IO;
        auto& camera = *ImguiCore::camera;

        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();

        float width = ImGui::GetWindowWidth();
        float height = ImGui::GetWindowHeight();
        ImVec2 position = ImGui::GetWindowPos();
        ImGuizmo::SetRect(position.x, position.y, width, height);

        sView = camera.view();
        sPerspective = camera.perspective();
        pMode = enableWorldMode ? ImGuizmo::WORLD : ImGuizmo::LOCAL;

        if (ImGuizmo::IsOver() || ImGuizmo::IsUsing()) {
//            info("Guizmo is over or used");
            ImguiCore::disableInput();
        } else {
//            info("Guizmo is not over or used");
            ImguiCore::enableInput();
        }

        sEntity = entity;
        renderGizmoTransform();
        renderGizmoLight();
    }

    void Gizmo::renderGizmoTransform() {
        auto* transform = sEntity.getComponent<Transform>();
        const float* viewPtr = glm::value_ptr(sView);
        const float* perspectivePtr = glm::value_ptr(sPerspective);

        if (transform) {
            float* transformPtr = glm::value_ptr(transform->value);
            float* translationPtr = glm::value_ptr(transform->translation);
            float* rotationPtr = glm::value_ptr(transform->rotation);
            float* scalePtr = glm::value_ptr(transform->scale);

            if (enableTranslation) {
                ImGuizmo::Manipulate(
                        viewPtr,
                        perspectivePtr,
                        ImGuizmo::TRANSLATE,
                        pMode,
                        transformPtr
                );
            }

            if (enableRotation) {
                ImGuizmo::Manipulate(
                        viewPtr,
                        perspectivePtr,
                        ImGuizmo::ROTATE,
                        pMode,
                        transformPtr
                );
            }

            if (enableScale) {
                ImGuizmo::Manipulate(
                        viewPtr,
                        perspectivePtr,
                        ImGuizmo::SCALE,
                        pMode,
                        transformPtr
                );
            }

            ImGuizmo::DecomposeMatrixToComponents(
                    transformPtr,
                    translationPtr,
                    rotationPtr,
                    scalePtr
            );
        }
    }

    void Gizmo::renderGizmoLight() {
        const float* viewPtr = glm::value_ptr(sView);
        const float* perspectivePtr = glm::value_ptr(sPerspective);
        bool updated = false;

        auto* phongLight = sEntity.getComponent<PhongLightComponent>();
        auto* directLight = sEntity.getComponent<DirectLightComponent>();
        auto* pointLight = sEntity.getComponent<PointLightComponent>();
        auto* spotLight = sEntity.getComponent<SpotLightComponent>();

        if (enableTranslation && phongLight) {
            glm::mat4 translate(1.0f);
            translate = glm::translate(translate, glm::vec3(phongLight->position));
            ImGuizmo::Manipulate(
                    viewPtr,
                    perspectivePtr,
                    ImGuizmo::TRANSLATE,
                    pMode,
                    glm::value_ptr(translate)
            );
            glm::vec4 newPosition = glm::vec4(translate[3]);
            updated = updated || (newPosition != phongLight->position);
            phongLight->position = newPosition;
        }

        if (enableTranslation && directLight) {
            glm::mat4 translate(1.0f);
            translate = glm::translate(translate, glm::vec3(directLight->position));
            ImGuizmo::Manipulate(
                    viewPtr,
                    perspectivePtr,
                    ImGuizmo::TRANSLATE,
                    pMode,
                    glm::value_ptr(translate)
            );
            glm::vec4 newPosition = glm::vec4(translate[3]);
            updated = updated || (newPosition != directLight->position);
            directLight->position = newPosition;
        }

        if (enableTranslation && pointLight) {
            glm::mat4 translate(1.0f);
            translate = glm::translate(translate, glm::vec3(pointLight->position));
            ImGuizmo::Manipulate(
                    viewPtr,
                    perspectivePtr,
                    ImGuizmo::TRANSLATE,
                    pMode,
                    glm::value_ptr(translate)
            );
            glm::vec4 newPosition = glm::vec4(translate[3]);
            updated = updated || (newPosition != pointLight->position);
            pointLight->position = newPosition;
        }

        if (enableTranslation && spotLight) {
            glm::mat4 translate(1.0f);
            translate = glm::translate(translate, glm::vec3(spotLight->position));
            ImGuizmo::Manipulate(
                    viewPtr,
                    perspectivePtr,
                    ImGuizmo::TRANSLATE,
                    pMode,
                    glm::value_ptr(translate)
            );
            glm::vec4 newPosition = glm::vec4(translate[3]);
            updated = updated || (newPosition != spotLight->position);
            spotLight->position = newPosition;
        }

        if (updated) {
            LightStorage::update();
        }
    }

}