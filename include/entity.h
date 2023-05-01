#pragma once

#include <scene.h>

namespace ecs {

    struct Entity {

        Entity() = default;

        Entity(Scene* scene) : scene(scene) {
            id = scene->create_entity();
        }

        inline EntityID get_id() const { return id; }

        inline void set_id(EntityID id) { this->id = id; }

        inline void set_scene(Scene* scene) { this->scene = scene; }

        inline const Scene* get_scene() const { return scene; }

        inline bool invalid() const { return id == InvalidEntity; }
        inline bool valid() const { return id != InvalidEntity; }

        template<typename T>
        T* get_component();

        template<typename T, typename... Args>
        T* add_component(Args &&... args);

        template<typename T>
        void remove_component();

        template<typename T>
        bool valid_component();

        template<typename T>
        bool invalid_component();

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

    template<typename T>
    bool Entity::valid_component() {
        return get_component<T>() != null;
    }

    template<typename T>
    bool Entity::invalid_component() {
        return get_component<T>() == null;
    }

}