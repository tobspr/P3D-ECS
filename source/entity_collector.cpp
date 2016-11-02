
#include "entity_collector.h"
#include "entity.h"

#include <iomanip>

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
        // TODO: Slow!
        if (std::find(_matching_entities.begin(), _matching_entities.end(), entity) == _matching_entities.end()) {
            _matching_entities.push_back(entity);
        }
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
        // Notice: it is not always sure that if the entity matches our mask, it is in our
        // container. For example, assuming a Collector which collects TransformComponents
        //   Entity* e = mgr->new_entity();
        //   e->new_component<TransformComponent>();
        //     ^ At this point, the entity already matches the collectors mask, but it will
        //       only be in the collectors entity list after the single_step call
        //   mgr->single_step(...);
        //
        // For this reason, we use the _if_present version of the erase method.
        vector_erase_fast_if_present(_matching_entities, entity);
        ECS_OUTPUT_DEBUG("Removed entity.");
    }
}

void EntityCollector::print_status()
{
    ECS_OUTPUT_DEBUG("  Collector with mask " << std::hex << _component_mask << std::dec << " and " << _matching_entities.size() << " entities:");
    for (Entity* e : _matching_entities) {
        ECS_OUTPUT_DEBUG("    - " << *e);
    }
}
