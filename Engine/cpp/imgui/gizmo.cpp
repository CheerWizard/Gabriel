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

    bool Gizmo::sLightUpdated = false;

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

        if (sEntity.validComponent<GizmoTransform>()) {
            renderGizmoTransform(sEntity.getComponent<Transform>());
        }

        else if (sEntity.validComponent<GizmoTransform2d>()) {
            renderGizmoTransform2d(sEntity.getComponent<Transform2d>());
        }

        else if (sEntity.validComponent<GizmoPhongLight>()) {
            renderGizmoPhongLight();
        }

        else if (sEntity.validComponent<GizmoDirectLight>()) {
            renderGizmoDirectLight();
        }

        else if (sEntity.validComponent<GizmoPointLight>()) {
            renderGizmoPointLight();
        }

        else if (sEntity.validComponent<GizmoSpotLight>()) {
            renderGizmoSpotLight();
        }

        else if (sEntity.validComponent<GizmoText2d>()) {
            renderGizmoText2d();
        }

        else if (sEntity.validComponent<GizmoText3d>()) {
            renderGizmoText3d();
        }

        if (sLightUpdated) {
            LightStorage::update();
        }
    }

    void Gizmo::renderGizmoTransform(Transform* transform) {
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

    void Gizmo::renderGizmoTransform2d(Transform2d* transform) {
        const float* viewPtr = glm::value_ptr(sView);
        const float* perspectivePtr = glm::value_ptr(sPerspective);

        if (transform) {
            float* transformPtr = glm::value_ptr(transform->value);
            float* translationPtr = glm::value_ptr(transform->translation);
            float* rotationPtr = &transform->rotation;
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

    void Gizmo::renderGizmoPhongLight() {
        const float* viewPtr = glm::value_ptr(sView);
        const float* perspectivePtr = glm::value_ptr(sPerspective);
        auto* phongLight = sEntity.getComponent<PhongLightComponent>();

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
            sLightUpdated = sLightUpdated || (newPosition != phongLight->position);
            phongLight->position = newPosition;
        }
    }

    void Gizmo::renderGizmoDirectLight() {
        const float* viewPtr = glm::value_ptr(sView);
        const float* perspectivePtr = glm::value_ptr(sPerspective);
        auto* directLight = sEntity.getComponent<DirectLightComponent>();

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
            sLightUpdated = sLightUpdated || (newPosition != directLight->position);
            directLight->position = newPosition;
        }
    }

    void Gizmo::renderGizmoPointLight() {
        const float* viewPtr = glm::value_ptr(sView);
        const float* perspectivePtr = glm::value_ptr(sPerspective);
        auto* pointLight = sEntity.getComponent<PointLightComponent>();

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
            sLightUpdated = sLightUpdated || (newPosition != pointLight->position);
            pointLight->position = newPosition;
        }
    }

    void Gizmo::renderGizmoSpotLight() {
        const float* viewPtr = glm::value_ptr(sView);
        const float* perspectivePtr = glm::value_ptr(sPerspective);
        auto* spotLight = sEntity.getComponent<SpotLightComponent>();

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
            sLightUpdated = sLightUpdated || (newPosition != spotLight->position);
            spotLight->position = newPosition;
        }
    }

    void Gizmo::renderGizmoText2d() {
        renderGizmoTransform2d(&sEntity.getComponent<Text2d>()->transform);
    }

    void Gizmo::renderGizmoText3d() {
        renderGizmoTransform(&sEntity.getComponent<Text3d>()->transform);
    }

}