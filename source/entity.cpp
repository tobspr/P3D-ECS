
#include "entity.h"

#include "entity_manager.h"

Entity::id_t Entity::next_id = 1000u;

void Entity::register_component(Component::id_t id, Component *ptr)
{
    _component_mask |= Component::to_bitmask(id);
    _components.emplace(id, ptr);
}

void Entity::register_entity() {
    manager->register_entity(this);
}
