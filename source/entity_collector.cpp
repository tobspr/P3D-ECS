
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
    if ((entity->get_component_mask() & _component_mask) == _component_mask) {
        _matching_entities.emplace(entity);
    }
};
