#include <entity.h>

namespace ecs {

    void Entity::free() {
        if (id != InvalidEntity)
            scene->remove_entity(id);
    }

}