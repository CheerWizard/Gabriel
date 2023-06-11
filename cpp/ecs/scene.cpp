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

    void Scene::serialize(BinaryStream& stream) {
        stream.addString(name);

        stream.add(mEntities);

        size_t componentTableSize = mComponentTable.size();
        stream.add(componentTableSize);
        for (auto& entry : mComponentTable) {
            ComponentID componentId = entry.first;
            stream.add(componentId);
            entry.second.serialize(componentId, stream);
        }
    }

    void Scene::deserialize(BinaryStream& stream) {
        free();

        stream.getString(name);

        stream.get(mEntities);

        size_t componentsSize = 0;
        stream.get(componentsSize);
        for (size_t i = 0 ; i < componentsSize ; i++) {
            ComponentID componentId = 0;
            stream.get(componentId);
            ComponentVector componentVector;
            componentVector.deserialize(componentId, stream);
            mComponentTable[componentId] = componentVector;
        }

        invalidateAllComponentAddresses();
    }

    void Scene::invalidateComponentAddresses(ComponentID componentId, ComponentSize componentSize) {
        for (const EntityID entityId : mEntities) {
            mComponentAddresses[entityId][componentId] = mComponentTable[componentId].getAddress(componentSize, entityId);
        }
    }

    void Scene::invalidateAllComponentAddresses() {
        for (auto& componentEntry : mComponentTable) {
            ComponentID componentId = componentEntry.first;
            if (componentId != InvalidComponent) {
                ComponentSize componentSize = ComponentMetaTable::get(componentId).SIZE;
                invalidateComponentAddresses(componentId, componentSize);
            }
        }
    }

    ComponentAddress ComponentVector::getAddress(ComponentSize componentSize, EntityID entityId) {
        size_t size = mComponents.size();
        size_t step = componentSize;
        for (size_t i = 0 ; i < size ; i += step) {
            ComponentAddress componentAddress = &mComponents[i];
            EntityID componentEntityId = *static_cast<EntityID*>(componentAddress);
            if (componentEntityId == entityId) {
                return componentAddress;
            }
        }
        return null;
    }

    void ComponentVector::free(ComponentID componentId) {
        ComponentMeta componentMeta = ComponentMetaTable::get(componentId);
        size_t size = mComponents.size();
        size_t step = componentMeta.SIZE;
        for (size_t i = 0 ; i < size ; i += step) {
            auto* component = (BaseComponent*) &mComponents[i];
            componentMeta.DESTROY(component);
        }
    }

    void ComponentVector::serialize(ComponentID componentId, BinaryStream& stream) {
        stream.add(mComponents);

        SerializableMeta* serializableMeta = SerializableMetaTable::get(componentId);
        if (serializableMeta) {
            size_t size = mComponents.size();
            size_t step = serializableMeta->SIZE;
            for (size_t i = 0 ; i < size ; i += step) {
                auto* component = (BaseComponent*) &mComponents[i];
                serializableMeta->SERIALIZE(component, stream);
            }
        }
    }

    void ComponentVector::deserialize(ComponentID componentId, BinaryStream& stream) {
        stream.get(mComponents);

        SerializableMeta* serializableMeta = SerializableMetaTable::get(componentId);
        if (serializableMeta) {
            size_t size = mComponents.size();
            size_t step = serializableMeta->SIZE;
            for (size_t i = 0 ; i < size ; i += step) {
                auto* component = (BaseComponent*) &mComponents[i];
                serializableMeta->DESERIALIZE(component, stream);
            }
        }
    }

}