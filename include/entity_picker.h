#pragma once

#include <polygon_visual.h>
#include <normal_visual.h>
#include <scene.h>

namespace gl {

    struct Pickable : ecs::Component {
        bool enable = false;
    };

    struct EntityPicker final {
        EntityPicker(ecs::Scene* scene, ecs::EntityID entity_id);
    };

}