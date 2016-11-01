
#include "entity_manager.h"
#include "perf_utility.h"

#include <algorithm>

EntityManager::EntityManager()
{
    _all_entities.reserve(10000);
    _entities_to_delete.reserve(100);
    _new_entities.reserve(1000);
}

EntityManager::~EntityManager()
{
    shutdown();
}

void EntityManager::register_entity(Entity* entity)
{
    ECS_OUTPUT_DEBUG("Registering new entity to manager: " << *entity);
    for (EntityCollector* collector : _collectors) {
        collector->consider_register(entity);
    }
}

Entity* EntityManager::new_entity()
{
    Entity* entity = new Entity(this);
    _new_entities.push_back(entity);
    ECS_OUTPUT_DEBUG("Constructed new entity: " << *entity);
    return entity;
}

void EntityManager::single_step(float dt)
{
    ECS_OUTPUT_DEBUG("Single step, dt = " << dt);

    ECS_OUTPUT_DEBUG("Deleting " << _entities_to_delete.size() << " entities from last frame ..");
    for (Entity* entity : _entities_to_delete)
    {
        vector_erase_fast(_all_entities, entity);
        for (EntityCollector* collector : _collectors)
        {
            collector->remove_entity(entity);
        }
        delete entity;
    }
    _entities_to_delete.clear();

    ECS_OUTPUT_DEBUG("Adding " << _new_entities.size()
                               << " new entities from last frame ..");
    for (Entity* entity : _new_entities) {
        ECS_OUTPUT_DEBUG("  -> Adding: " << entity);
        entity->register_entity();
        _all_entities.push_back(entity);
    }
    _new_entities.clear();
}

void EntityManager::shutdown()
{
    ECS_OUTPUT_DEBUG("Shutting down entity manager");

	ECS_OUTPUT_DEBUG("Deleting entities");
    for(Entity* entity : _all_entities)
        delete entity;

    for(Entity* entity : _new_entities)
        delete entity;
    
    for(Entity* entity: _entities_to_delete)
        delete entity;

	ECS_OUTPUT_DEBUG("Deleting collectors");
	for (EntityCollector* collector : _collectors)
		delete collector;

}

void EntityManager::delete_entity(Entity* entity)
{
    ECS_OUTPUT_DEBUG("Deletion of " << *entity << " was requested");
    auto it = std::find(_new_entities.begin(), _new_entities.end(), entity);
    if (it != _new_entities.end())
    {
        ECS_OUTPUT_DEBUG("Entity was not registered yet. Can just delete it again");
        _new_entities.erase(it);
        delete entity;
    } else {
        ECS_OUTPUT_DEBUG("Entity was already registered, queuing to delete it next frame");
        _entities_to_delete.push_back(entity);
    }
}
