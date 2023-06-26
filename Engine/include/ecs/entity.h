#pragma once

#include <ecs/scene.h>
#include <ecs/common.h>

namespace gl {

    struct GABRIEL_API Entity {

        Entity() = default;

        Entity(Scene* scene);

        Entity(EntityID id, Scene* scene) : id(id), scene(scene) {}

        virtual void free();

        [[nodiscard]] inline EntityID getId() const { return id; }

        inline void setId(EntityID id) { this->id = id; }

        inline void setScene(Scene* scene) { this->scene = scene; }

        [[nodiscard]] inline const Scene* getScene() const { return scene; }

        [[nodiscard]] inline bool invalid() const { return id == InvalidEntity; }
        [[nodiscard]] inline bool valid() const { return id != InvalidEntity; }

        template<typename T>
        T* getComponent();

        template<typename T, typename... Args>
        T* addComponent(Args &&... args);

        template<typename T>
        void removeComponent();

        template<typename T>
        bool validComponent();

        template<typename T>
        bool invalidComponent();

        inline bool operator ==(const Entity& other) const {
            return id == other.getId();
        }

    protected:
        EntityID id = InvalidEntity;
        Scene* scene = null;
    };

    template<typename T>
    T* Entity::getComponent() {
        return scene->getComponent<T>(id);
    }

    template<typename T, typename... Args>
    T* Entity::addComponent(Args &&... args) {
        return scene->addComponent<T>(id, std::forward<Args>(args)...);
    }

    template<typename T>
    void Entity::removeComponent() {
        scene->removeComponent<T>(id);
    }

    template<typename T>
    bool Entity::validComponent() {
        return getComponent<T>() != null;
    }

    template<typename T>
    bool Entity::invalidComponent() {
        return getComponent<T>() == null;
    }

}