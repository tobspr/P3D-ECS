#pragma once
#ifndef P3DECS_ENTITY_H
#define P3DECS_ENTITY_H

#include "config_module.h"
#include "component.h"
#include "memory_pool.h"

#include <stdint.h>
#include <cassert>
#include <utility>
#include <iostream>

class EntityManager;
class EntityCollector;

class Entity final {
    friend class EntityManager;
    friend class EntityCollector;
    friend class MemoryPool<Entity>;

    using id_t = uint_fast64_t;
    using component_pair_t = std::pair<Component::id_t, Component*>;
    static id_t next_id;

public:
    // Required for unittests
    static inline void reset_id_pool() { next_id = 1000u; };

    ~Entity();
    inline id_t get_id() const { return _id; };
    inline size_t get_num_components() const { return _components.size(); };
    inline Component::bitmask_t get_component_mask() const
    {
        return _component_mask;
    }

    template <typename T>
    T& get_component()
    {
        assert(has_component<T>());
        Component::id_t id = Component::extract_id<T>();
        for (component_pair_t& id_and_ptr : _components) {
            if (id_and_ptr.first == id)
                return *static_cast<T*>(id_and_ptr.second);
        }

		assert(false); // should never happen
		return *static_cast<T*>(nullptr);
    };

    template <typename T>
    const T& get_component() const
    {
        return const_cast<Entity*>(this)->get_component<T>();
    };

    template <typename T>
    bool has_component() const
    {
        return (_component_mask & Component::to_bitmask(Component::extract_id<T>())) != 0u;
    };

    template <typename T>
    void remove_component()
    {
        // TODO: assert(!_deleted)
        // TODO: if (T == TransformComponent) assert(TransformComponent.children.empty)
    }

#ifndef INTERROGATE
    template <typename T, typename... Args>
    T& new_component(Args&&... args)
    {
        // TODO: assert(!_deleted)
        assert(!has_component<T>());
        T* component = MemoryPool<T>::new_object(this, std::forward<Args>(args)...);
        register_component(Component::extract_id<T>(), component);
        return *component;
    };
#endif

    void remove();

private:
    inline explicit Entity(EntityManager* manager);

    void on_registered_by_manager();
    void register_component(Component::id_t id, Component* ptr);

    bool is_registered_to_collector(EntityCollector* collector);
    void on_registered_to_collector(EntityCollector* collector);
    void on_deregistered_from_collector(EntityCollector* collector);

    EntityManager* _manager;

    bool _registered;
    bool _deleted;
    id_t _id;
    Component::bitmask_t _component_mask;

    std::vector<component_pair_t> _components;
    std::vector<EntityCollector*> _registered_collectors;
};

#include "entity.I"

inline ostream&
operator<<(ostream& stream, const Entity& entity)
{
    return stream << "[Entity @" << &entity << ", id=" << entity.get_id()
                  << ", components=" << entity.get_num_components() << "]";
};

#endif
