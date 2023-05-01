#include <entity_control.h>

namespace gl {

    bool EntityControl::select(double x, double y) {
        if (!(flags & Select)) {
            return false;
        }

        RayCollider ray = camera->shoot_ray(x ,y);
        SphereCollider* closest_sphere = null;
        float closest_distance = FLOAT_MAX;

        scene->each_component<SphereCollider>([&ray, &closest_distance, &closest_sphere](SphereCollider* sphere) {
            auto response = CollisionDetection::test(ray, *sphere);
            if (response.hit) {
                float distance = min(response.d1, response.d2);
                if (distance < closest_distance) {
                    closest_distance = distance;
                    closest_sphere = sphere;
                }
            }
        });

        if (closest_sphere) {
            ecs::EntityID entity_id = closest_sphere->entity_id;
            selected_entity.set_scene(scene);
            selected_entity.set_id(entity_id);

            auto* selectable = selected_entity.get_component<Selectable>();
            if (selectable && selectable->callback) {
                selectable->callback(selected_entity, x, y);
            }

            return true;
        }

        return false;
    }

    void EntityControl::drag(double x, double y) {
        if (!(flags & Drag)) {
            return;
        }

        ViewRay mouse_view_ray = camera->raycast_view(x, y);
        // entity World -> View
        auto& entity_world_position = selected_entity.get_component<Transform>()->translation;
        WorldRay entity_world_ray(entity_world_position.x, entity_world_position.y, entity_world_position.z, 1.0f);
        glm::mat4 camera_view = camera->view();
        ViewRay entity_view_ray = entity_world_ray.view_space(camera_view);
        // intersect rays mouse View and entity View by Z plane
        ViewRay intersect_view_ray = {
                mouse_view_ray.vector.x * entity_view_ray.vector.z,
                mouse_view_ray.vector.y * entity_view_ray.vector.z,
                mouse_view_ray.vector.z * entity_view_ray.vector.z,
                1.0f
        };
        // entity View -> World
        entity_world_position = intersect_view_ray.world_space(camera_view).vec3();
    }

    void EntityControl::hover(double x, double y) {
        if (!(flags & Hover)) {
            return;
        }
    }

}