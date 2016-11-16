
#include "movement_system.h"

#include "transform_component.h"
#include "physics_component.h"

#include "entity_manager.h"
#include "entity_collector.h"

MovementSystem::MovementSystem(EntityManager* manager)
  : EntitySystem(manager) {
  _relevant_entities = manager->new_collector<TransformComponent, PhysicsComponent>();
};

void
MovementSystem::process(float dt) {
  for (Entity* entity : *_relevant_entities) {
    entity->get_component<TransformComponent>().set_pos({ 1.0, 2.0, 5.0 });
  }
};
