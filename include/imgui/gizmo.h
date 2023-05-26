#pragma once

namespace gl {

    component(GizmoTransformComponent) {};
    component(GizmoPhongLight) {};
    component(GizmoDirectLight) {};
    component(GizmoPointLight) {};
    component(GizmoSpotLight) {};

    struct Gizmo final {

        static bool enableTranslation;
        static bool enableRotation;
        static bool enableScale;
        static bool enableWorldMode;

        static void render(const Entity& entity);

    private:
        static void renderGizmoTransform();
        static void renderGizmoLight();

    private:
        static Entity sEntity;
        static glm::mat4 sView;
        static glm::mat4 sPerspective;
    };

}