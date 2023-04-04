#pragma once

#include <primitives.h>

#include <unordered_map>
#include <vector>
#include <functional>

namespace ecs {

    typedef u32 EntityID;
    static constexpr EntityID InvalidEntity = 0;
    typedef size_t ComponentID;
    typedef void* ComponentAddress;

    struct Component {
        EntityID entity_id = InvalidEntity;
    };

    struct ComponentVector final {

        template<typename T>
        inline size_t get_size() const { return components.size() / sizeof(T); }

        template<typename T>
        inline size_t get_capacity() const { return components.capacity() / sizeof(T); }

        template<typename T>
        inline T* get(int i) { return (T*) (components[i * sizeof(T)]); }

        inline bool has_capacity() const { return components.size() < components.capacity(); }

        template<typename T>
        T* get(EntityID entity_id);

        template<typename T>
        void reserve(size_t new_capacity);

        template<typename T>
        void resize(size_t new_size);

        template<typename T, typename... Args>
        T* emplace(Args &&... args);

        template<typename T, typename... Args>
        void update(T* component, Args &&... args);

        template<typename T>
        void erase(int index);

        template<typename T>
        void erase(T* component);

        template<typename T>
        void for_each(const std::function<void(T*)>& iterate_function);

    private:
        std::vector<u8> components;
    };

    template<typename T>
    void ComponentVector::reserve(size_t new_capacity) {
        // allocate memory for components
        components.reserve(sizeof(T) * new_capacity);
    }

    template<typename T>
    void ComponentVector::resize(size_t new_size) {
        components.resize(sizeof(T) * new_size);
    }

    template<typename T, typename... Args>
    T* ComponentVector::emplace(Args &&... args) {
        // initialize component with arguments without memory allocations
        size_t components_size = components.size();
        components.resize(components_size + sizeof(T));
        T* component = new(&components[components_size]) T(std::forward<Args>(args)...);
        return component;
    }

    template<typename T, typename... Args>
    void ComponentVector::update(T* component, Args &&... args) {
        // re-initialize component with arguments without memory allocations
        component->~T();
        new(component) T(std::forward<Args>(args)...);
    }

    template<typename T>
    void ComponentVector::erase(int index) {
        auto begin = components.begin() + (index * sizeof(T));
        auto end = begin + sizeof(T);
        components.erase(begin, end);
    }

    template<typename T>
    void ComponentVector::erase(T* component) {
        size_t size = components.size();
        size_t step = sizeof(T);
        for (size_t i = 0 ; i < size ; i += step) {
            ComponentAddress src = &components[i];
            ComponentAddress target = component;
            if (src == target) {
                component->~T();
                erase<T>(i / sizeof(T));
                break;
            }
        }
    }

    template<typename T>
    void ComponentVector::for_each(const std::function<void(T*)>& iterate_function) {
        size_t size = components.size();
        size_t step = sizeof(T);
        for (size_t i = 0 ; i < size ; i += step) {
            T* component = (T*) &components[i];
            iterate_function(component);
        }
    }

    template<typename T>
    T* ComponentVector::get(EntityID entity_id) {
        size_t size = components.size();
        size_t step = sizeof(T);
        for (size_t i = 0 ; i < size ; i += step) {
            T* component = (T*) &components[i];
            if (component->entity_id == entity_id) {
                return component;
            }
        }
        return null;
    }

    struct Scene final {

        EntityID create_entity();

        void add_entity(EntityID entity_id);

        void remove_entity(EntityID entity_id);

        inline std::vector<EntityID>& get_entities() { return entities; }

        template<typename T>
        void reserve_components(size_t capacity);

        template<typename T, typename... Args>
        T* add_component(EntityID entity_id, Args &&... args);

        template<typename T>
        void remove_component(EntityID entity_id);

        template<typename T>
        T* get_component(EntityID entity_id);

        template<typename T>
        ComponentVector& get_components();

        template<typename T>
        void each_component(const std::function<void(T*)>& iterate_function);

    private:
        template<typename T>
        ComponentID get_component_id();

        template<typename T>
        void invalidate_component_addresses();

    private:
        EntityID entity_id_generator = 0;
        std::vector<EntityID> entities;
        std::unordered_map<EntityID, std::unordered_map<ComponentID, ComponentAddress>> component_addresses;
        std::unordered_map<ComponentID , ComponentVector> component_table;
    };

    template<typename T>
    ComponentID Scene::get_component_id() {
        return typeid(T).hash_code();
    }

    template<typename T>
    void Scene::reserve_components(size_t capacity) {
        ComponentID cid = get_component_id<T>();
        component_table[cid] = {};
        component_table[cid].reserve<T>(capacity);
    }

    template<typename T, typename... Args>
    T* Scene::add_component(EntityID entity_id, Args&&... args) {
        ComponentID cid = get_component_id<T>();
        T* component = (T*) component_addresses[entity_id][cid];
        // update component if it already exists
        if (component) {
            component_table[cid].update<T>(component, std::forward<Args>(args)...);
        }
        // add new component if none exists
        else {
            // reallocate more memory if capacity exceeds
            auto& component_vector = component_table[cid];
            if (!component_vector.has_capacity()) {
                component_vector.reserve<T>(component_vector.get_size<T>() * 2 + 1);
                // because component storage memory changed, we need to invalidate all addresses that use it
                invalidate_component_addresses<T>();
            }
            component = component_vector.emplace<T>(std::forward<Args>(args)...);
            component_addresses[entity_id][cid] = component;
        }

        component->entity_id = entity_id;

        return component;
    }

    template<typename T>
    void Scene::remove_component(EntityID entity_id) {
        ComponentID cid = get_component_id<T>();
        T* component = (T*) component_addresses[entity_id][cid];
        if (!component) {
            print_err("Scene::remove_component: component for entity " << entity_id << "does not exist");
            return;
        }
        // remove component from storage
        component_table[cid].erase<T>(component);
        component_addresses[entity_id][cid] = null;
    }

    template<typename T>
    T* Scene::get_component(EntityID entity_id) {
        ComponentID cid = get_component_id<T>();
        return (T*) component_addresses[entity_id][cid];
    }

    template<typename T>
    ComponentVector& Scene::get_components() {
        ComponentID cid = get_component_id<T>();
        return component_table[cid];
    }

    template<typename T>
    void Scene::each_component(const std::function<void(T*)>& iterate_function) {
        ComponentID cid = get_component_id<T>();
        component_table[cid].for_each<T>(iterate_function);
    }

    template<typename T>
    void Scene::invalidate_component_addresses() {
        ComponentID cid = get_component_id<T>();
        for (EntityID entity_id : entities) {
            component_addresses[entity_id][cid] = component_table[cid].get<T>(entity_id);
        }
    }

}