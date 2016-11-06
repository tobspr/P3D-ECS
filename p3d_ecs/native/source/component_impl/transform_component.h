#pragma once

#ifndef P3D_ECS_TRANSFORMCOMPONENT_H
#define P3D_ECS_TRANSFORMCOMPONENT_H

#include "transform_component_meta.h"
#include "perf_utility.h"

class TransformComponent final : public TransformComponentMeta {
public:
  DEFINE_COMPONENT(TransformComponent);

  inline void set_pos(const LVecBase3f& pos) {
    _pos = pos;
    recompute_mat();
  }

  inline void set_scale(const LVecBase3f& scale) {
    _scale = scale;
    recompute_mat();
  }

  inline void set_hpr(const LVecBase3f& hpr) {
    _hpr = hpr;
    recompute_mat();
  }

  inline void set_mat(const LMatrix4f& mat) {
    _mat = mat;
    _is_dirty = true; /* todo: reconstruct pos and so on? or maybe set a flag like
                      _custom_mat */
  }

  void set_parent(Entity* entity);

private:
  inline void recompute_mat() { /* todo */
    _is_dirty = true;
  }

  inline void unregister_child(Entity* entity) { vector_erase_fast(_children, entity); };
  inline void register_child(Entity* entity) {
    /* TODO: assert(!_children.contains(entity)) */
    _children.push_back(entity);
  }

  void unregister_from_parent();

  inline TransformComponent(Entity* entity)
    : superclass(entity){};
};
#endif
