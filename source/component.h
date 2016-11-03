#pragma once
#ifndef P3DECS_COMPONENT_H
#define P3DECS_COMPONENT_H

#include "config_module.h"

#include <stdint.h>

class Entity;
class EntityManager;

struct Component {
    friend class EntityManager;
    
    using id_t = uint_fast8_t;
    using bitmask_t = uint_fast64_t;

    virtual void deleter() = 0;

    static inline bitmask_t to_bitmask(id_t id) { return ((bitmask_t)1u) << id; }

    template <typename T>
    static inline id_t extract_id()
    {
        return T::component_id;
    }

protected:
    Component(Entity* entity)
        : _entity(entity){};

    Entity* _entity;
};

#define DEFINE_COMPONENT(ClassName) \
    static const id_t component_id; \
    friend class MemoryPool<ClassName>; \
	friend class EntityManager; \
    virtual void deleter() override { \
        MemoryPool<ClassName>::delete_object_from_upcast<Component>(this); \
        this->~ClassName(); \
    }

#define IMPLEMENT_COMPONENT(name, id) \
    const Component::id_t name::component_id = id;

#endif
