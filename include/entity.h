#pragma once

#include <scene.h>

namespace ecs {

    struct Entity {

        Entity() = default;

        Entity(Scene* scene) : scene(scene) {
            id = scene->create_entity();
        }

        inline EntityID get_id() const { return id; }

        template<typename T>
        T* get_component();

        template<typename T, typename... Args>
        T* add_component(Args &&... args);

        template<typename T>
        void remove_component();

    protected:
        EntityID id = InvalidEntity;
        Scene* scene = null;
    };

    template<typename T>
    T* Entity::get_component() {
        return scene->get_component<T>(id);
    }

    template<typename T, typename... Args>
    T* Entity::add_component(Args &&... args) {
        return scene->add_component<T>(id, std::forward<Args>(args)...);
    }

    template<typename T>
    void Entity::remove_component() {
        scene->remove_component<T>(id);
    }

}