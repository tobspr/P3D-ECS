#pragma once

#ifndef P3D_ECS_TRANSFORMCOMPONENT_H
#define P3D_ECS_TRANSFORMCOMPONENT_H

#include "transform_component_meta.h"

class TransformComponent final : public TransformComponentMeta {
public:
  DEFINE_COMPONENT(TransformComponent);

  inline void set_pos(const LVecBase3f& pos);
  inline void set_scale(const LVecBase3f& scale);
  inline void set_hpr(const LVecBase3f& hpr);
  inline void set_matrix(const LMatrix4f& mat);

  void set_parent(Entity* entity);
  virtual void on_deserialization_finished() override;
  virtual void on_removed() override;

private:
  void recompute_matrix();
  void reconstruct_from_matrix();
  void recompute_absolute_matrix();

  inline void unregister_child(Entity* entity);
  inline void register_child(Entity* entity);
  inline void on_parent_gone();

  inline void reregister_child_after_deserialization(Entity* entity);

  void fix_parent_child_relations();

  inline TransformComponent(Entity* entity)
    : superclass(entity){};
};

#include "transform_component.I"

#endif
