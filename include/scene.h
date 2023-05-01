#pragma once

#include <component.h>

#include <unordered_map>
#include <vector>
#include <functional>

namespace ecs {

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
        void erase_at(int index, size_t type_size);

        template<typename T>
        void erase(EntityID entity_id);

        template<typename T>
        void for_each(const std::function<void(T*)>& iterate_function);

        void free(ComponentID component_id);

    private:
        std::vector<u8> components;
    };

    template<typename T>
    void ComponentVector::reserve(size_t new_capacity) {
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
    void ComponentVector::erase_at(int index, size_t type_size) {
        auto begin = components.begin() + (index * type_size);
        auto end = begin + type_size;
        components.erase(begin, end);
    }

    template<typename T>
    void ComponentVector::erase(EntityID entity_id) {
        size_t size = components.size();
        size_t type_size = sizeof(T);
        for (size_t i = 0 ; i < size ; i += type_size) {
            T* component = (T*) &components[i];
            if (component->entity_id == entity_id) {
                component->~T();
                erase_at<T>(i / type_size, type_size);
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

        ~Scene() {
            free();
        }

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

        template<typename T>
        size_t component_size();

        void free();

    private:
        template<typename T>
        void invalidate_component_addresses();

    private:
        EntityID entity_id_generator = 0;
        std::vector<EntityID> entities;
        std::unordered_map<EntityID, std::unordered_map<ComponentID, ComponentAddress>> component_addresses;
        std::unordered_map<ComponentID , ComponentVector> component_table;
    };

    template<typename T>
    void Scene::reserve_components(size_t capacity) {
        component_table[T::ID] = {};
        component_table[T::ID].reserve<T>(capacity);
    }

    template<typename T, typename... Args>
    T* Scene::add_component(EntityID entity_id, Args&&... args) {
        ComponentID cid = T::ID;
        T* component = (T*) component_addresses[entity_id][T::ID];
        // update component if it already exists
        if (component) {
            component_table[T::ID].update<T>(component, std::forward<Args>(args)...);
        }
        // add new component if none exists
        else {
            // reallocate more memory if capacity exceeds
            auto& component_vector = component_table[T::ID];
            if (!component_vector.has_capacity()) {
                component_vector.reserve<T>(component_vector.get_size<T>() * 2 + 1);
                // because component storage memory changed, we need to invalidate all addresses that use it
                invalidate_component_addresses<T>();
            }
            component = component_vector.emplace<T>(std::forward<Args>(args)...);
            component_addresses[entity_id][T::ID] = component;
        }

        component->entity_id = entity_id;

        return component;
    }

    template<typename T>
    void Scene::remove_component(EntityID entity_id) {
        ComponentAddress component_address = component_addresses[entity_id][T::ID];
        if (component_address == null) {
            print_err("Scene::remove_component(): component for entity " << entity_id << "does not exist");
            return;
        }
        // remove component from storage
        component_table[T::ID].erase<T>(entity_id);
        component_addresses[entity_id][T::ID] = null;
    }

    template<typename T>
    T* Scene::get_component(EntityID entity_id) {
        return (T*) component_addresses[entity_id][T::ID];
    }

    template<typename T>
    ComponentVector& Scene::get_components() {
        return component_table[T::ID];
    }

    template<typename T>
    void Scene::each_component(const std::function<void(T*)>& iterate_function) {
        component_table[T::ID].for_each<T>(iterate_function);
    }

    template<typename T>
    void Scene::invalidate_component_addresses() {
        for (EntityID entity_id : entities) {
            component_addresses[entity_id][T::ID] = component_table[T::ID].get<T>(entity_id);
        }
    }

    template<typename T>
    size_t Scene::component_size() {
        return component_table[T::ID].get_size();
    }

}