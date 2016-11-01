
#include "entity_collector.h"

EntityCollector::EntityCollector(component_list components)
    : _components(components)
{
    Entity::component_mask_t mask;
    for (Component::id_t component_id : _components)
    {
        mask |= Component::to_bitmask(component_id);
    }
    _component_mask = mask;
}
