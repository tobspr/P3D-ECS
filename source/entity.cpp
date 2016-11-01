
#include "entity.h"

#include "entity_manager.h"

Entity::id_t Entity::next_id = 1000u;

void Entity::register_component(Component::id_t id, Component *ptr)
{
    _component_mask |= ((component_mask_t)1u) << id;
    _components.emplace(id, ptr);
}

void Entity::register_entity() {
    for (auto it : _components)
    {
        _manager->register_entity_with_component(it.first, this);
    }
}
