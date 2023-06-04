#pragma once

#include <features/transform.h>

namespace gl {

    component(GizmoTransform) {};
    component(GizmoTransform2d) {};

    component(GizmoPhongLight) {};
    component(GizmoDirectLight) {};
    component(GizmoPointLight) {};
    component(GizmoSpotLight) {};

    component(GizmoText2d) {};
    component(GizmoText3d) {};

    struct Gizmo final {

        static bool enableTranslation;
        static bool enableRotation;
        static bool enableScale;
        static bool enableWorldMode;

        static void render(const Entity& entity);

    private:
        static void renderGizmoTransform(Transform* transform);
        static void renderGizmoTransform2d(Transform2d* transform);

        static void renderGizmoPhongLight();
        static void renderGizmoDirectLight();
        static void renderGizmoPointLight();
        static void renderGizmoSpotLight();

        static void renderGizmoText2d();
        static void renderGizmoText3d();

    private:
        static Entity sEntity;
        static glm::mat4 sView;
        static glm::mat4 sPerspective;
        static bool sLightUpdated;
    };

}