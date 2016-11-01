#pragma once
#ifndef P3DECS_COMPONENT_H
#define P3DECS_COMPONENT_H

#include "config_module.h"

#include <stdint.h>

struct Component {
    using id_t = uint_fast8_t;
    using bitmask_t = uint_fast64_t;

    static inline bitmask_t to_bitmask(id_t id) { return ((bitmask_t)1u) << id; }

    template <typename T>
    static inline id_t extract_id()
    {
        return T::component_id;
    }
};

#define DEFINE_COMPONENT() static const Component::id_t component_id;
#define IMPLEMENT_COMPONENT(name, id) \
    const Component::id_t name::component_id = id;

#endif
