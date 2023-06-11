#pragma once

#include <ecs/component.h>

namespace gl {

    struct ComponentVector final {

        template<typename T>
        [[nodiscard]] inline size_t getSize() const { return mComponents.size() / T::META.SIZE; }

        template<typename T>
        [[nodiscard]] inline size_t getCapacity() const { return mComponents.capacity() / T::META.SIZE; }

        template<typename T>
        inline T* get(int i) { return (T*) (mComponents[i * T::META.SIZE]); }

        [[nodiscard]] inline bool hasCapacity() const { return mComponents.size() < mComponents.capacity(); }

        template<typename T>
        T* get(EntityID entityId);

        ComponentAddress getAddress(ComponentSize componentSize, EntityID entityId);

        template<typename T>
        void reserve(size_t newCapacity);

        template<typename T>
        void resize(size_t newSize);

        template<typename T, typename... Args>
        T* emplace(Args &&... args);

        template<typename T, typename... Args>
        void update(T* component, Args &&... args);

        template<typename T>
        void eraseAt(int index, size_t typeSize);

        template<typename T>
        void erase(EntityID entityId);

        template<typename T>
        void forEach(const std::function<void(T*)>& iterateFunction);

        void free(ComponentID componentId);

        inline std::vector<u8>& data() {
            return mComponents;
        }

        inline bool notEmpty() {
            return mComponents.size() > 0;
        }

        inline bool empty() {
            return mComponents.size() <= 0;
        }

        void serialize(ComponentID componentId, BinaryStream& stream);
        void deserialize(ComponentID componentId, BinaryStream& stream);

    private:
        std::vector<u8> mComponents;
    };

    template<typename T>
    void ComponentVector::reserve(size_t newCapacity) {
        mComponents.reserve(T::META.SIZE * newCapacity);
    }

    template<typename T>
    void ComponentVector::resize(size_t newSize) {
        mComponents.resize(T::META.SIZE * newSize);
    }

    template<typename T, typename... Args>
    T* ComponentVector::emplace(Args &&... args) {
        // initialize component with arguments without memory allocations
        size_t componentsSize = mComponents.size();
        mComponents.resize(componentsSize + T::META.SIZE);
        T* component = new(&mComponents[componentsSize]) T(std::forward<Args>(args)...);
        return component;
    }

    template<typename T, typename... Args>
    void ComponentVector::update(T* component, Args &&... args) {
        // re-initialize component with arguments without memory allocations
        component->~T();
        new(component) T(std::forward<Args>(args)...);
    }

    template<typename T>
    void ComponentVector::eraseAt(int index, size_t typeSize) {
        auto begin = mComponents.begin() + (index * typeSize);
        auto end = begin + typeSize;
        mComponents.erase(begin, end);
    }

    template<typename T>
    void ComponentVector::erase(EntityID entityId) {
        size_t size = mComponents.size();
        size_t typeSize = T::META.SIZE;
        for (size_t i = 0 ; i < size ; i += typeSize) {
            T* component = (T*) &mComponents[i];
            if (component->entityId == entityId) {
                component->~T();
                eraseAt<T>(i / typeSize, typeSize);
                break;
            }
        }
    }

    template<typename T>
    void ComponentVector::forEach(const std::function<void(T*)>& iterateFunction) {
        size_t size = mComponents.size();
        size_t step = T::META.SIZE;
        for (size_t i = 0 ; i < size ; i += step) {
            T* component = (T*) &mComponents[i];
            iterateFunction(component);
        }
    }

    template<typename T>
    T* ComponentVector::get(EntityID entityId) {
        size_t size = mComponents.size();
        size_t step = T::META.SIZE;
        for (size_t i = 0 ; i < size ; i += step) {
            T* component = (T*) &mComponents[i];
            if (component->entityId == entityId) {
                return component;
            }
        }
        return null;
    }

    struct Scene {
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