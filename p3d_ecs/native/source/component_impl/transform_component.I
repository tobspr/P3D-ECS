
#include "perf_utility.h"

inline void
TransformComponent::set_pos(const LVecBase3f& pos) {
  _pos = pos;
  recompute_matrix();
}

inline void
TransformComponent::set_scale(const LVecBase3f& scale) {
  _scale = scale;
  recompute_matrix();
}

inline void
TransformComponent::set_hpr(const LVecBase3f& hpr) {
  _hpr = hpr;
  recompute_matrix();
}

inline void
TransformComponent::set_matrix(const LMatrix4f& mat) {
  _matrix = mat;
  reconstruct_from_matrix();
}

inline void
TransformComponent::unregister_child(Entity* entity) {
  ::p3d_ecs::performance::vector_erase(_children, entity);
};

inline void
TransformComponent::on_parent_gone() {
  // Parent was either deleted or has no longer a TransformComponent
  _parent = nullptr;
}

inline void
TransformComponent::register_child(Entity* entity) {
  ECS_ASSERT(!::p3d_ecs::performance::vector_contains(_children, entity));
  _children.push_back(entity);
}

inline void
TransformComponent::reregister_child_after_deserialization(Entity* entity) {
  // See on_deserialization_finished
  if (!::p3d_ecs::performance::vector_contains(_children, entity))
    _children.push_back(entity);
}
