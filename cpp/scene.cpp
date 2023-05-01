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

    void Scene::free() {
        for (auto& component_vector : component_table) {
            component_vector.second.free(component_vector.first);
        }
        entities.clear();
        component_addresses.clear();
        component_table.clear();
    }

    void ComponentVector::free(ComponentID component_id) {
//        size_t size = components.size();
//        size_t step = sizeof(T);
//        for (size_t i = 0 ; i < size ; i += step) {
//            T* component = (T*) &components[i];
//            component->~T();
//        }
    }

}