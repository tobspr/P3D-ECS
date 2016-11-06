#pragma once

// AUTOGENERATED. DO NOT EDIT!
// Instead modify the component meta file.

#ifndef P3D_ECS_PHYSICSCOMPONENT_METACLS_H
#define P3D_ECS_PHYSICSCOMPONENT_METACLS_H

#include "config_module.h"
#include "component.h"
#include "perf_utility.h"
#include "memory_pool.h"
#include "luse.h"

class Entity;
class EntityManager;
class PhysicsComponent;

class PhysicsComponentMeta : public Component {
  using superclass = PhysicsComponentMeta;

  public:
    DEFINE_COMPONENT_BASE();

    // AUTOGEN:: accessors
    inline bool get_is_static() const { return _is_static; }
    inline void set_is_static(bool val) { _is_static = val; }

    inline float get_mass() const { return _mass; }
    inline void set_mass(float val) { _mass = val; }


    // AUTOGEN:: serialization
    virtual void serialize(PlainTextSerializer* serializer) const override;

    virtual bool data_equals(const Component &other) const override;
  protected:
    // AUTOGEN:: constructor
    inline PhysicsComponentMeta(Entity* entity) : Component(entity)
      , _is_static(false)
      , _mass(10.0)
      {};

    // AUTOGEN:: internal members
    bool _is_static;
    float _mass;

    // AUTOGEN:: member names for plain text serialization
    static const char* is_static_CSTR;
    static const char* mass_CSTR;

};
#endif
