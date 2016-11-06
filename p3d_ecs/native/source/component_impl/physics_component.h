#pragma once

#ifndef P3D_ECS_PHYSICS_COMPONENT_H
#define P3D_ECS_PHYSICS_COMPONENT_H

#include "physics_component_meta.h"
#include "perf_utility.h"

class PhysicsComponent final : public PhysicsComponentMeta {
public:
  DEFINE_COMPONENT(PhysicsComponent);

private:
  inline PhysicsComponent(Entity* entity)
    : superclass(entity){};
};
#endif
