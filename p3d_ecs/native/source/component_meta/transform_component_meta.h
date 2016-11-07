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
#include <vector>

class Entity;
class EntityManager;
class TransformComponent;

class TransformComponentMeta : public Component {
  protected:
    using superclass = TransformComponentMeta;

  public:
    DEFINE_COMPONENT_BASE();

    // AUTOGEN:: accessors
    inline const std::vector<Entity*>& get_children() const { return _children; }

    inline const LVecBase3f& get_hpr() const { return _hpr; }

    inline bool get_is_dirty() const { return _is_dirty; }

    inline const LMatrix4f& get_mat() const { return _mat; }

    inline Entity* get_parent() const { return _parent; }

    inline const LVecBase3f& get_pos() const { return _pos; }

    inline const LVecBase3f& get_scale() const { return _scale; }


    // AUTOGEN:: serialization
    virtual void serialize(PlainTextSerializer* serializer) const override;

    virtual bool data_equals(const Component &other) const override;
  protected:
    // AUTOGEN:: constructor
    inline TransformComponentMeta(Entity* entity) : Component(entity)
      , _children()
      , _hpr()
      , _is_dirty(false)
      , _mat(LMatrix4f::ident_mat())
      , _parent(nullptr)
      , _pos()
      , _scale(1, 1, 1)
      {};

    // AUTOGEN:: internal members
    std::vector<Entity*> _children;
    LVecBase3f _hpr;
    bool _is_dirty;
    LMatrix4f _mat;
    Entity* _parent;
    LVecBase3f _pos;
    LVecBase3f _scale;

    // AUTOGEN:: member names for plain text serialization
    static const char* children_CSTR;
    static const char* hpr_CSTR;
    static const char* is_dirty_CSTR;
    static const char* mat_CSTR;
    static const char* parent_CSTR;
    static const char* pos_CSTR;
    static const char* scale_CSTR;

};
#endif
