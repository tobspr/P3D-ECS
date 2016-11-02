
#include "entity.h"
#include "entity_manager.h"

Entity::id_t Entity::next_id = 1000u;

Entity::~Entity()
{
    // TODO: Add check if we were manually deleted and print an error in that case
    ECS_ON_DELETE("Entity");
    for (auto it : _components)
        delete it.second;
}

void Entity::register_component(Component::id_t id, Component* ptr)
{
    _component_mask |= Component::to_bitmask(id);
    _components.emplace(id, ptr);
    if (_registered) {
        // In case the entity is already registered, reregister it to make
        // sure it is registered on all collectors
        _manager->on_component_added(this);
    }
}

void Entity::register_entity()
{
    if (!_registered)
        _registered = true;
    _manager->register_entity(this);
}

void Entity::remove()
{
    if (_deleted) {
        // TODO: Warning about this entity being deleted twice
        return;
    }
    _manager->delete_entity(this);
    _deleted = true;
}