
#include "transform_component.h"

#include "entity.h"

void TransformComponent::set_parent(Entity *new_parent) {
  ECS_OUTPUT_SPAM("Setting parent to " << *new_parent);
  assert(new_parent == nullptr ||
         new_parent->has_component<TransformComponent>());

  _is_dirty = true;
  // Deregister from old parent
  if (_parent)
    _parent->get_component<TransformComponent>().unregister_child(_entity);

  // Register to new parent
  _parent = new_parent;
  if (_parent)
    _parent->get_component<TransformComponent>().register_child(_entity);
}

void TransformComponent::unregister_from_parent() {
  if (_parent)
    _parent->get_component<TransformComponent>().unregister_child(_entity);
}
