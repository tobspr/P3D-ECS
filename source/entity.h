#pragma once
#ifndef P3ECS_ENTITY_H
#define P3ECS_ENTITY_H

#include "config_module.h"
#include "component.h"

#include <stdint.h>
#include <cassert>

class Entity
{
public:
    using id_t = uint_fast64_t;
    using component_mask_t = uint_fast64_t;

    inline Entity();
    inline id_t get_id() const { return _id; };

    template < typename T >
    T* get_component() {
        assert(has_component<T>());
        return T(); // todo
    };

    template < typename T >
    const T* get_component() const {
        return const_cast<Entity*>(this)->get_component<T>();
    };

    template < typename T >
    bool has_component() const {
        return (_component_mask & (1 << T::class_id)) != 0u;
    }

private:
    id_t _id;
    component_mask_t _component_mask;

    static id_t next_id;
};


#include "entity.I"

#endif
