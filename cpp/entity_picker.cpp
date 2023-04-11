#include <entity_picker.h>

namespace gl {

    EntityPicker::EntityPicker(ecs::Scene* scene, ecs::EntityID entity_id) {
        print("Entity ID = " << entity_id);

        if (entity_id == ecs::InvalidEntity) {
            print_err("Picked invalid entity");
            return;
        } else {
            auto* pickable = scene->get_component<Pickable>(entity_id);
            if (pickable) {
                pickable->enable = !pickable->enable;
                auto* drawable = scene->get_component<DrawableElements>(entity_id);
                auto* transform = scene->get_component<Transform>(entity_id);
                if (pickable->enable) {
                    scene->add_component<PolygonVisual>(entity_id);
                    scene->add_component<NormalVisual>(entity_id);
                } else {
                    scene->remove_component<PolygonVisual>(entity_id);
                    scene->remove_component<NormalVisual>(entity_id);
                }
            } else {
                print_err("Entity " << entity_id << " is not Pickable");
            }
        }
    }

}