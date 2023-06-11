#pragma once

#include <io/serialization.h>

namespace gl {

    typedef u32 EntityID;
    static constexpr EntityID InvalidEntity = 0;
    typedef size_t ComponentID;
    static constexpr ComponentID InvalidComponent = 0;
    typedef size_t ComponentSize;
    typedef void* ComponentAddress;

    struct BaseComponent;

    typedef u32 (*ComponentCreateFunction)(std::vector<u8>& componentData, EntityID entityId, BaseComponent* component);
    typedef void (*ComponentDestroyFunction)(BaseComponent* component);
    typedef void (*ComponentSerializeFunction)(BaseComponent* component, BinaryStream& stream);
    typedef void (*ComponentDeserializeFunction)(BaseComponent* component, BinaryStream& stream);

    struct ComponentMeta;
    struct SerializableMeta;

    struct ComponentMetaTable final {

        static const ComponentMeta& get(ComponentID componentId);
        static std::unordered_map<ComponentID, ComponentMeta>& getTable();

    private:
        static std::unordered_map<ComponentID, ComponentMeta>* sTable;
    };

    struct SerializableMetaTable final {

        static SerializableMeta* get(ComponentID componentId);
        static std::unordered_map<ComponentID, SerializableMeta>& getTable();

    private:
        static std::unordered_map<ComponentID, SerializableMeta>* sTable;
    };

    struct ComponentMeta {
        ComponentID ID = 0;
        ComponentSize SIZE = 0;
        ComponentCreateFunction CREATE;
        ComponentDestroyFunction DESTROY;

        ComponentMeta() = default;

        ComponentMeta(
                ComponentID id,
                ComponentSize size,
                ComponentCreateFunction createFunction,
                ComponentDestroyFunction destroyFunction
        ) :
        ID(id),
        SIZE(size),
        CREATE(createFunction),
        DESTROY(destroyFunction)
        {
            ComponentMetaTable::getTable()[id] = *this;
        }
    };

    struct SerializableMeta : ComponentMeta {
        ComponentSerializeFunction SERIALIZE;
        ComponentDeserializeFunction DESERIALIZE;

        SerializableMeta() = default;

        SerializableMeta(
                ComponentID id,
                ComponentSize size,
                ComponentCreateFunction createFunction,
                ComponentDestroyFunction destroyFunction,
                ComponentSerializeFunction serializeFunction,
                ComponentDeserializeFunction deserializeFunction
        ) :
        ComponentMeta(id, size, createFunction, destroyFunction),
        SERIALIZE(serializeFunction),
        DESERIALIZE(deserializeFunction)
        {
            SerializableMetaTable::getTable()[id] = *this;
        }
    };

    struct BaseComponent {
        EntityID entityId = InvalidEntity;
    };

    template<typename Derived>
    struct Component : BaseComponent {
        static const ComponentMeta META;
    };

    template<typename Derived>
    u32 createComponent(std::vector<u8>& componentData, EntityID entityId, BaseComponent* component) {
        u32 componentIndex = componentData.size();
        componentData.resize(componentIndex + sizeof(Derived));
        auto* newComponent = new(&componentData[componentIndex]) Derived(*(Derived*) component);
        newComponent->entityId = entityId;
        return componentIndex;
    }

    template<typename Derived>
    void destroyComponent(BaseComponent* component) {
        ((Derived*) component)->~Derived();
    }

    template<typename Derived>
    const ComponentMeta Component<Derived>::META(
            typeid(Derived).hash_code(),
            sizeof(Derived),
            createComponent<Derived>,
            destroyComponent<Derived>
    );

    template<typename Derived>
    struct SerializableComponent : BaseComponent {
        static const SerializableMeta META;
    };

    template<typename Derived>
    void serializeComponent(BaseComponent* component, BinaryStream& stream) {
        ((Derived*) component)->serialize(stream);
    }

    template<typename Derived>
    void deserializeComponent(BaseComponent* component, BinaryStream& stream) {
        ((Derived*) component)->deserialize(stream);
    }

    template<typename Derived>
    const SerializableMeta SerializableComponent<Derived>::META(
            typeid(Derived).hash_code(),
            sizeof(Derived),
            createComponent<Derived>,
            destroyComponent<Derived>,
            serializeComponent<Derived>,
            deserializeComponent<Derived>
    );

    #define component(type) struct type : gl::Component<type>
    #define component_serializable(type) struct type : gl::SerializableComponent<type>

}