#pragma once

#include <primitives.h>

namespace ecs {

    typedef u32 EntityID;
    static constexpr EntityID InvalidEntity = 0;
    typedef size_t ComponentID;
    typedef void* ComponentAddress;

    template<typename Derived>
    struct Component {
        static const ComponentID ID;
        EntityID entity_id = InvalidEntity;
    };

    template<typename Derived>
    const ComponentID Component<Derived>::ID(typeid(Derived).hash_code());

    #define component(type) struct type : ecs::Component<type>

}