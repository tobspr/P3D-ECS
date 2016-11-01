#pragma once
#ifndef P3DECS_COMPONENT_H
#define P3DECS_COMPONENT_H

#include "config_module.h"

#include <stdint.h>

#define DEFINE_COMPONENT() static const Component::id_t component_id;
#define IMPLEMENT_COMPONENT(name, id) const Component::id_t name::component_id = id;

struct Component
{
    using id_t = uint_fast8_t;
};

#endif
