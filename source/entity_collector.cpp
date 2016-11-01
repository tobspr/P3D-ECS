
#include "entity_collector.h"
#include "entity.h"

EntityCollector::EntityCollector(const component_list& components)
    : _components(components)
{
    ECS_ON_CREATE("EntityCollector");
    Component::bitmask_t mask = 0u;
    for (Component::id_t component_id : _components) {
        mask |= Component::to_bitmask(component_id);
    }
    _component_mask = mask;
    ECS_OUTPUT_DEBUG(
        "Constructed new collector with bitmask: " << _component_mask);
}

void EntityCollector::consider_register(Entity* entity)
{
    if (entity_fits(entity)) {
        _matching_entities.push_back(entity);
    }
};

bool EntityCollector::entity_fits(Entity* entity) const
{
    return (entity->get_component_mask() & _component_mask) == _component_mask;
}

void EntityCollector::remove_entity(Entity* entity)
{
    if (entity_fits(entity)) {
        ECS_OUTPUT_DEBUG("Removing entity " << *entity << " from collector");
        vector_erase_fast(_matching_entities, entity);
    }
}
