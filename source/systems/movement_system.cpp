
#include "movement_system.h"

#include "all_components.h"
#include "entity_manager.h"
#include "entity_collector.h"

MovementSystem::MovementSystem(EntityManager* manager)
	: EntitySystem(manager)
{
    _relevant_entities = manager->new_collector<TransformComponent, PhysicsComponent>();
};

void MovementSystem::process(float dt)
{
    ECS_OUTPUT_DEBUG("MovementSystem::process(dt = " << dt << ")");

    ECS_OUTPUT_DEBUG("Our relevant collector has " << _relevant_entities->size() << " entries.");

    for (Entity *entity : *_relevant_entities)
    {
        ECS_OUTPUT_DEBUG("  -> Updating transform component of " << *entity);
        entity->get<TransformComponent>().pos.set_x(5.0);
    }
};
