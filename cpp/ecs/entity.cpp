#include <ecs/entity.h>

#ifdef IMGUI
#include <features/transform.h>
#include <debugging/visuals.h>
#endif

namespace gl {

    Entity::Entity(Scene *scene) : scene(scene) {
        id = scene->createEntity();
        addComponent<Tag>("Entity " + std::to_string(id));
#ifdef IMGUI
//        addComponent<Transform>();
//        addComponent<PolygonVisual>();
//        addComponent<NormalVisual>();
#endif
    }

    void Entity::free() {
        if (id != InvalidEntity)
            scene->removeEntity(id);
    }

}