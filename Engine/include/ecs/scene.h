#pragma once

#include <ecs/component_vector.h>

namespace gl {

    struct GABRIEL_API Scene {
        std::string name;

        Scene(const std::string& name = "Untitled") : name(name) {}

        ~Scene() {
            free();
        }

        EntityID createEntity();

        void addEntity(EntityID entityId);

        void removeEntity(EntityID entityId);

        inline std::vector<EntityID>& getEntities() { return mEntities; }

        template<typename T>
        void reserveComponents(size_t capacity);

        template<typename T, typename... Args>
        T* addComponent(EntityID entityId, Args &&... args);

        template<typename T>
        void removeComponent(EntityID entityId);

        template<typename T>
        T* getComponent(EntityID entityId);

        template<typename T>
        ComponentVector& getComponents();

        template<typename T>
        void eachComponent(const std::function<void(T*)>& iterateFunction);

        template<typename T>
        size_t componentSize();

        void free();

        void serialize(BinaryStream& stream);
        void deserialize(BinaryStream& stream);

    private:
        template<typename T>
        void invalidateComponentAddresses();

        void invalidateComponentAddresses(ComponentID componentId, ComponentSize componentSize);

        void invalidateAllComponentAddresses();

    private:
        static constexpr float RESERVE_WEIGHT = 0.75;

        EntityID mEntityIdGenerator = 0;
        std::vector<EntityID> mEntities;
        std::unordered_map<EntityID, std::unordered_map<ComponentID, ComponentAddress>> mComponentAddresses;
        std::unordered_map<ComponentID , ComponentVector> mComponentTable;
    };

    template<typename T>
    void Scene::reserveComponents(size_t capacity) {
        mComponentTable[T::META.ID] = {};
        mComponentTable[T::META.ID].reserve<T>(capacity);
    }

    template<typename T, typename... Args>
    T* Scene::addComponent(EntityID entityId, Args&&... args) {
        T* component = (T*) mComponentAddresses[entityId][T::META.ID];
        // update component if it already exists
        if (component) {
            mComponentTable[T::META.ID].update<T>(component, std::forward<Args>(args)...);
        }
        // add new component if none exists
        else {
            // reallocate more memory if capacity exceeds
            auto& componentVector = mComponentTable[T::META.ID];
            if (!componentVector.hasCapacity()) {
                componentVector.reserve<T>(componentVector.getSize<T>() * 2 + 1);
                // because component storage memory changed, we need to invalidate all addresses that use it.
                // it may be expensive operation when scene has a lot of entities,
                // but it depends only on component storage capacity and reserve weight variables.
                invalidateComponentAddresses<T>();
            }
            component = componentVector.emplace<T>(std::forward<Args>(args)...);
            mComponentAddresses[entityId][T::META.ID] = component;
        }

        component->entityId = entityId;

        return component;
    }

    template<typename T>
    void Scene::removeComponent(EntityID entityId) {
        ComponentAddress& componentAddress = mComponentAddresses[entityId][T::META.ID];
        if (componentAddress == null) {
            error("Component for entity {0} does not exist", entityId);
            return;
        }
        // remove component from storage
        mComponentTable[T::META.ID].erase<T>(entityId);
        componentAddress = null;
    }

    template<typename T>
    T* Scene::getComponent(EntityID entityId) {
        return (T*) mComponentAddresses[entityId][T::META.ID];
    }

    template<typename T>
    ComponentVector& Scene::getComponents() {
        return mComponentTable[T::META.ID];
    }

    template<typename T>
    void Scene::eachComponent(const std::function<void(T*)>& iterateFunction) {
        mComponentTable[T::META.ID].forEach<T>(iterateFunction);
    }

    template<typename T>
    void Scene::invalidateComponentAddresses() {
        if (T::META.ID != InvalidComponent) {
            invalidateComponentAddresses(T::META.ID, T::META.SIZE);
        }
    }

    template<typename T>
    size_t Scene::componentSize() {
        return mComponentTable[T::META.ID].getSize();
    }

}