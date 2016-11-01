
#include "entity.h"
#include "entity_manager.h"

Entity::id_t Entity::next_id = 1000u;

void Entity::register_component(Component::id_t id, Component* ptr) {
  _component_mask |= Component::to_bitmask(id);
  _components.emplace(id, ptr);
  if (_registered) {
    // In case the entity is already registered, reregister it to make
    // sure it is registered on all collectors
    register_entity();
  }
}

void Entity::register_entity() {
  if (!_registered)
    _registered = true;
  _manager->register_entity(this);
}
