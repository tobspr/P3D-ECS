#pragma once
#ifndef P3DECS_ENTITY_H
#define P3DECS_ENTITY_H

#include "config_module.h"
#include "component.h"

#include <stdint.h>
#include <cassert>
#include <utility>

class EntityManager;

class Entity
{
    friend class EntityManager;

  public:
    using id_t = uint_fast64_t;
    using component_mask_t = uint_fast64_t;

    inline id_t get_id() const { return _id; };

    template <typename T>
    T *get_component()
    {
        assert(has_component<T>());
        Component* component = _components.find(T::component_id)->second;
        return static_cast<T*>(component); // assumed to be safe
    };

    template <typename T>
    const T *get_component() const
    {
        return const_cast<Entity *>(this)->get_component<T>();
    };

    template <typename T>
    bool has_component() const
    {
        return (_component_mask & (((component_mask_t)1u) << T::component_id)) != 0u;
    };

    #ifndef INTERROGATE
    template <typename T, typename... Args>
    void add_component(Args && ...args)
    {
        assert(!has_component<T>());
        T *component = new T(std::forward<Args>(args)...);
        register_component(T::component_id, component);
    };
    #endif

  private:
    inline explicit Entity(EntityManager *manager);

    void register_entity();
    void register_component(Component::id_t id, Component *ptr);

    id_t _id;
    component_mask_t _component_mask;
    EntityManager *_manager;

    std::map<Component::id_t, Component *> _components;
    static id_t next_id;
};

#include "entity.I"

#endif
