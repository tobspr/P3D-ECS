
#include "entity_collector.h"
#include "entity.h"

#include <iomanip>

EntityCollector::EntityCollector(const component_list_t& components)
    : _components(components)
{
    ECS_ON_CREATE("EntityCollector");
    Component::bitmask_t mask = 0u;
    for (Component::id_t component_id : _components) {
        mask |= Component::to_bitmask(component_id);
    }
    _component_mask = mask;
    ECS_OUTPUT_SPAM(
        "Constructed new collector with bitmask: " << _component_mask);
}

EntityCollector::EntityIterator::EntityIterator(iter_t pos, iter_t end, Component::bitmask_t target_mask)
    : _pos(pos)
    , _end(end)
    , _target_mask(target_mask)
{
    inc_until_valid();
}

bool EntityCollector::EntityIterator::is_valid_entity(Entity* entity) const
{
    return (entity->get_component_mask() & _target_mask) == _target_mask && !entity->is_flagged_for_deletion();
}

void EntityCollector::EntityIterator::inc_until_valid()
{
    while (_pos != _end && !is_valid_entity(*_pos)) {
        ++_pos;
    }
}

EntityCollector::EntityIterator& EntityCollector::EntityIterator::operator++()
{
    ++_pos;
    inc_until_valid();
    return *this;
};

void EntityCollector::consider_register(Entity* entity)
{
    if (entity_fits(entity)) {
        if (!entity->is_registered_to_collector(this)) {
            _matching_entities.push_back(entity);
            entity->on_registered_to_collector(this);
        }
    }
};

bool EntityCollector::entity_fits(Entity* entity) const
{
    return (entity->get_component_mask() & _component_mask) == _component_mask;
}

void EntityCollector::consider_remove_entity(Entity* entity)
{
    if (entity_fits(entity)) {
        if (entity->is_registered_to_collector(this)) {
            remove_entity(entity);
        }
    }
}

void EntityCollector::print_status()
{
    ECS_OUTPUT_DEBUG("  Collector with mask " << std::hex << _component_mask << std::dec << " and " << _matching_entities.size() << " entities:");
    for (Entity* e : _matching_entities) {
        ECS_OUTPUT_DEBUG("    - " << *e);
    }
}

void EntityCollector::remove_entity(Entity* entity)
{
    ECS_OUTPUT_SPAM("Removing entity " << *entity << " from collector");
    vector_erase_fast(_matching_entities, entity);
    entity->on_deregistered_from_collector(this);
}