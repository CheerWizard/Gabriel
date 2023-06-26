#include <ecs/component_vector.h>

namespace gl {

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