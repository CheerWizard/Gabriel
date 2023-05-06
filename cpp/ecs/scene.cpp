#include <ecs/scene.h>

namespace gl {

    EntityID Scene::createEntity() {
        EntityID newEntity = ++mEntityIdGenerator;
        addEntity(newEntity);
        return newEntity;
    }

    void Scene::addEntity(EntityID id) {
        mEntities.emplace_back(id);
    }

    void Scene::removeEntity(EntityID id) {
        mEntities.erase(std::find(mEntities.begin(), mEntities.end(), id));
    }

    void Scene::free() {
        for (auto& componentVector : mComponentTable) {
            componentVector.second.free(componentVector.first);
        }
        mEntities.clear();
        mComponentAddresses.clear();
        mComponentTable.clear();
    }

    void ComponentVector::free(ComponentID componentId) {
//        size_t size = components.size();
//        size_t step = sizeof(T);
//        for (size_t i = 0 ; i < size ; i += step) {
//            T* component = (T*) &components[i];
//            component->~T();
//        }
    }

}