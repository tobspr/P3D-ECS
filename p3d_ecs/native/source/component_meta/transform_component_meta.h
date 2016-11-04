#pragma once

// AUTOGENERATED. DO NOT EDIT!
// Instead modify the component meta file.

#ifndef P3D_ECS_TRANSFORMCOMPONENT_METACLS_H
#define P3D_ECS_TRANSFORMCOMPONENT_METACLS_H

#include "config_module.h"
#include "component.h"
#include "perf_utility.h"
#include "memory_pool.h"
#include "luse.h"

// AUTOGEN:: aditional includes
#include "entity_ref.h"
#include <vector>

class Entity;
class EntityManager;
class YAMLSerializer;
class TransformComponent;

class TransformComponentMeta : public Component {
  using superclass = TransformComponentMeta;

  public:
    DEFINE_COMPONENT_BASE();

    // AUTOGEN:: accessors
    inline const std::vector<Entity*>& get_children() const { return _children; }

    inline const LVecBase3f& get_hpr() const { return _hpr; }

    inline bool get_is_dirty() const { return _is_dirty; }

    inline const LMatrix4f& get_mat() const { return _mat; }

    inline const EntityRef& get_parent() const { return _parent; }

    inline const LVecBase3f& get_pos() const { return _pos; }

    inline const LVecBase3f& get_scale() const { return _scale; }


    // AUTOGEN:: serialization
    void serialize(YAMLSerializer* serializer) const;

  protected:
    // AUTOGEN:: constructor
    inline TransformComponentMeta(Entity* entity) : Component(entity)
      , _is_dirty(false)
      , _mat(LMatrix4f::ident_mat())
      , _scale(1, 1, 1)
      {};

    // AUTOGEN:: internal members
    std::vector<Entity*> _children;
    LVecBase3f _hpr;
    bool _is_dirty;
    LMatrix4f _mat;
    EntityRef _parent;
    LVecBase3f _pos;
    LVecBase3f _scale;
};
#endif
