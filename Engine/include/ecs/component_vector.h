#pragma once

#include <ecs/component.h>

namespace gl {

    struct GABRIEL_API ComponentVector final {

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

}