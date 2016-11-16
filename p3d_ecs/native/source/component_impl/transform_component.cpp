
#include "transform_component.h"
#include "entity.h"
#include "output.h"

#include "compose_matrix.h"

void
TransformComponent::set_parent(Entity* new_parent) {
  ECS_ASSERT(new_parent == nullptr || new_parent->has_component<TransformComponent>());

  _abs_matrix_is_dirty = true;

  // Deregister from old parent
  if (_parent)
    _parent->get_component<TransformComponent>().unregister_child(_entity);

  // Register to new parent
  _parent = new_parent;
  if (_parent)
    _parent->get_component<TransformComponent>().register_child(_entity);
}

void
TransformComponent::recompute_matrix() {
  compose_matrix(_matrix, _scale, LVecBase3f(0, 0, 0), _hpr, _pos, CS_default);
  recompute_absolute_matrix();
}

void
TransformComponent::reconstruct_from_matrix() {
  decompose_matrix(_matrix, _scale, _hpr, _pos, CS_default);
}

void
TransformComponent::on_deserialization_finished() {
  fix_parent_child_relations();
  reconstruct_from_matrix();
  recompute_absolute_matrix();
}

void
TransformComponent::fix_parent_child_relations() {
  if (_parent != nullptr) {
    // We just finished deserialization, and now it might be that our parent
    // does not know about this. For an example of this situation, see
    // testsuite_serialization.cpp:Parent/Child: Serialize and deserialize child.
    // In that case, the <childs> parent is <root>, but the <root>'s list of children
    // is empty, because it can not know about the children. So, to avoid that,
    // reregister on the parent.
    if (!_parent->has_component<TransformComponent>()) {
      ECS_EMIT_WARNING(SER_0001, "Parent specified without TransformComponent, ignoring parent.");
      _parent = nullptr;
    } else {
      _parent->get_component<TransformComponent>().reregister_child_after_deserialization(_entity);
    }
  }

  std::vector<Entity*> bad_children;

  for (Entity* child : _children) {
    // Same as above, now we have to check that all of our children know about
    // their new parent
    ECS_ASSERT(child != nullptr); // Should never, ever, happen

    if (!child->has_component<TransformComponent>()) {
      ECS_EMIT_WARNING(SER_0003, "Child " << *child
                                          << " no longer has a TransformComponent, ignoring it.");
      bad_children.push_back(child);
    } else {
      TransformComponent& child_component = child->get_component<TransformComponent>();

      // Check if the childs parent matches
      if (child_component.get_parent() != _entity) {
        if (child_component.get_parent() == nullptr) {
          // The child does not know about us yet, but we can simply fix that.
          // We can't use set_parent() though, since that would register the child
          // to our child list again.
          child_component._parent = _entity;
        } else {
          // We have a problem, the child is parented to *another* entity. This can
          // happen if between the serialization and deserialization of this entity,
          // the child was reparented.
          ECS_EMIT_WARNING(SER_0002,
                           "Serialization: Registered child "
                             << *child << " was reparented to " << *child_component.get_parent()
                             << " in the meantime! Respecting new parent, and ignoring child");
          bad_children.push_back(child);
        }
      }
    }
  }

  // Remove children which no longer belong to us
  for (Entity* child : bad_children) {
    ::p3d_ecs::performance::vector_erase(_children, child);
  }
}

void
TransformComponent::on_removed() {

  // Assuming we only have a scene graph, then the references to our children
  // would most likely be removed, and so we would have floating entities with
  // no parent. Instead, the user should call remove_recursive() on the entity
  // to also remove any assigned children.
  if (!_children.empty()) {
    ECS_EMIT_WARNING(
      SER_0004,
      "Removed transform component while still having children, might leave unused entities!");
    for (Entity* child : _children) {
      child->get_component<TransformComponent>().on_parent_gone();
    }
  }

  // Unregister from old parent
  if (_parent != nullptr) {
    set_parent(nullptr);
  }
}

void
TransformComponent::recompute_absolute_matrix() {
  // TODO
}
