#pragma once
#ifndef P3DECS_ENTITY_H
#define P3DECS_ENTITY_H

#include "config_module.h"
#include "component.h"

#include <stdint.h>
#include <cassert>
#include <utility>
#include <iostream>

class EntityManager;

class Entity final {
    friend class EntityManager;

    using id_t = uint_fast64_t;
    static id_t next_id;

public:
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
        Component* component = _components.find(Component::extract_id<T>())->second;
        return *static_cast<T*>(component); // has to be safe
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

    template < typename T >
    void remove_component()  {
        // TODO: assert(!_deleted)
        // TODO: if (T == TransformComponent) assert(TransformComponent.children.empty)
    }

#ifndef INTERROGATE
    template <typename T, typename... Args>
    void new_component(Args&&... args)
    {
        // TODO: assert(!_deleted)
        assert(!has_component<T>());
        T* component = new T(this, std::forward<Args>(args)...);
        register_component(Component::extract_id<T>(), component);
    };
#endif

    void remove();

private:
    inline explicit Entity(EntityManager* manager);

    void register_entity();
    void register_component(Component::id_t id, Component* ptr);

    EntityManager* _manager;

    bool _registered;
    bool _deleted;
    id_t _id;
    Component::bitmask_t _component_mask;

    std::map<Component::id_t, Component*> _components;
};

#include "entity.I"

inline ostream&
operator<<(ostream& stream, const Entity& entity)
{
    return stream << "[Entity @" << &entity << ", id=" << entity.get_id()
                  << ", components=" << entity.get_num_components() << "]";
};

#endif
