#include <ecs/entity.h>

namespace gl {

    void Entity::free() {
        if (id != InvalidEntity)
            scene->removeEntity(id);
    }

}