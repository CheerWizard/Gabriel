#pragma once

#include <primitives.h>

namespace ecs {

    typedef u32 EntityID;
    static constexpr EntityID InvalidEntity = 0;
    typedef size_t ComponentID;
    typedef void* ComponentAddress;

    struct Component {
        EntityID entity_id = InvalidEntity;
    };

}