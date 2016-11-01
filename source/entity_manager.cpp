
#include "entity_manager.h"

void EntityManager::register_entity(Entity* entity) {
  ECS_OUTPUT_DEBUG("Registering new entity to manager: " << *entity);
  for (EntityCollector* collector : _collectors) {
    collector->consider_register(entity);
  }
}

Entity* EntityManager::new_entity() {
  Entity* entity = new Entity(this);
  _new_entities.push_back(entity);
  ECS_OUTPUT_DEBUG("Constructed new entity: " << *entity);
  return entity;
}

void EntityManager::single_step(float dt) {
  ECS_OUTPUT_DEBUG("Single step, dt = " << dt);

  ECS_OUTPUT_DEBUG("Adding " << _new_entities.size()
                             << " new entities from last frame ..");
  for (Entity* entity : _new_entities) {
    ECS_OUTPUT_DEBUG("  -> Adding: " << entity);
    entity->register_entity();
  }
  _new_entities.clear();
}
