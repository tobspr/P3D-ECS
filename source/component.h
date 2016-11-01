#pragma once
#ifndef P3ECS_COMPONENT_H
#define P3ECS_COMPONENT_H


#include "config_module.h"

#include <stdint.h>

using ComponentUniqueID = uint_fast8_t;

class Component
{
public:
    static const ComponentUniqueID class_id;
};

#endif
