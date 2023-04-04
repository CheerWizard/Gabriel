#include <scene.h>

namespace ecs {

    EntityID Scene::create_entity() {
        EntityID new_entity = ++entity_id_generator;
        add_entity(new_entity);
        return new_entity;
    }

    void Scene::add_entity(EntityID id) {
        entities.emplace_back(id);
    }

    void Scene::remove_entity(EntityID id) {
        entities.erase(std::find(entities.begin(), entities.end(), id));
    }

}