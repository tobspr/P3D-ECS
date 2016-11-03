
#include "entity.h"
#include "entity_manager.h"
#include "perf_utility.h"

#include <algorithm>

Entity::id_t Entity::next_id = 1000u;

Entity::~Entity()
{
    // TODO: Add check if we were manually deleted and print an error in that case
    ECS_ON_DELETE("Entity");
    for (auto it : _components)
    {
        it.second->deleter();
    }
}

void Entity::register_component(Component::id_t id, Component* ptr)
{
    _component_mask |= Component::to_bitmask(id);
    _components.emplace_back(id, ptr);
    if (_registered) {
        // In case the entity is already registered, reregister it to make
        // sure it is registered on all collectors
        _manager->on_component_added(this);
    }
}

void Entity::on_registered_by_manager()
{
    _registered = true;
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

void Entity::on_registered_to_collector(EntityCollector* collector)
{
    if (!is_registered_to_collector(collector))
        _registered_collectors.push_back(collector);
}

bool Entity::is_registered_to_collector(EntityCollector* collector)
{
    return std::find(_registered_collectors.begin(), _registered_collectors.end(), collector) != _registered_collectors.end();
}

void Entity::on_deregistered_from_collector(EntityCollector* collector)
{
    vector_erase_fast(_registered_collectors, collector);
}
