#include <control/entity_control.h>

namespace gl {

    bool EntityControl::select(double x, double y) {
        if (!(flags & Select)) {
            return false;
        }

        RayCollider ray = camera->shootRay(x ,y);
        SphereCollider* closestSphere = null;
        float closestDistance = FLOAT_MAX;

        scene->eachComponent<SphereCollider>([&ray, &closestDistance, &closestSphere](SphereCollider* sphere) {
            auto response = CollisionDetection::test(ray, *sphere);
            if (response.hit) {
                float distance = min(response.d1, response.d2);
                if (distance < closestDistance) {
                    closestDistance = distance;
                    closestSphere = sphere;
                }
            }
        });

        if (closestSphere) {
            EntityID entityId = closestSphere->entityId;
            mSelectedEntity.setScene(scene);
            mSelectedEntity.setId(entityId);

            auto* selectable = mSelectedEntity.getComponent<Selectable>();
            if (selectable && selectable->callback) {
                selectable->callback(mSelectedEntity, x, y);
            }

            return true;
        }

        return false;
    }

    void EntityControl::drag(double x, double y) {
        if (!(flags & Drag)) {
            return;
        }

        ViewRay mouseViewRay = camera->raycastView(x, y);
        // entity World -> View
        auto& entityWorldPosition = mSelectedEntity.getComponent<Transform>()->translation;
        WorldRay entityWorldRay(entityWorldPosition.x, entityWorldPosition.y, entityWorldPosition.z, 1.0f);
        glm::mat4 cameraView = camera->view();
        ViewRay entityViewRay = entityWorldRay.viewSpace(cameraView);
        // intersect rays mouse View and entity View by Z plane
        ViewRay intersectViewRay = {
                mouseViewRay.vector.x * entityViewRay.vector.z,
                mouseViewRay.vector.y * entityViewRay.vector.z,
                mouseViewRay.vector.z * entityViewRay.vector.z,
                1.0f
        };
        // entity View -> World
        entityWorldPosition = intersectViewRay.worldSpace(cameraView).vec3();
    }

    void EntityControl::hover(double x, double y) {
        if (!(flags & Hover)) {
            return;
        }
    }

}