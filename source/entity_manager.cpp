
#include "entity_manager.h"
#include "perf_utility.h"
#include "all_components.h"
#include "memory_pool.h"

#include <algorithm>

EntityManager::EntityManager()
{
    _all_entities.reserve(10000);
    _entities_to_delete.reserve(100);
    _new_entities.reserve(1000);
}

EntityManager::~EntityManager()
{
    reset();
}

void EntityManager::register_entity(Entity* entity)
{
    ECS_OUTPUT_DEBUG("Registering new entity to manager: " << *entity);
    entity->on_registered_by_manager();
    for (EntityCollector* collector : _collectors) {
        collector->consider_register(entity);
    }
}

Entity* EntityManager::new_entity()
{
    Entity* entity = MemoryPool<Entity>::new_object(this);
    _new_entities.push_back(entity);
    ECS_OUTPUT_DEBUG("Constructed new entity: " << *entity);
    return entity;
}

void EntityManager::single_step(float dt)
{
    ECS_OUTPUT_DEBUG("Single step, dt = " << dt);

    // Delete queued entities
    ECS_OUTPUT_DEBUG("Deleting " << _entities_to_delete.size() << " entities from last frame ..");
    while (!_entities_to_delete.empty()) {
        Entity* entity = _entities_to_delete.back();
        _entities_to_delete.pop_back();
        do_delete_entity(entity, EntityDeletionContext::InUpdate);
    }

    // Add new entities
    ECS_OUTPUT_DEBUG("Adding " << _new_entities.size()
                               << " new entities from last frame ..");
    for (Entity* entity : _new_entities) {
        ECS_OUTPUT_DEBUG("  -> Adding: " << *entity);
        register_entity(entity);
        _all_entities.push_back(entity);
    }
    _new_entities.clear();

    // Reevaluate changed entities
    for (Entity* entity : _entities_with_new_components)
    {
        register_entity(entity);
    }

}

void EntityManager::reset()
{
    ECS_OUTPUT_DEBUG("Shutting down entity manager");

    ECS_OUTPUT_DEBUG("Deleting entities");
    for (Entity* entity : _all_entities)
        MemoryPool<Entity>::delete_object(entity);

    for (Entity* entity : _new_entities)
        MemoryPool<Entity>::delete_object(entity);

    for (Entity* entity : _entities_to_delete)
        MemoryPool<Entity>::delete_object(entity);

    ECS_OUTPUT_DEBUG("Deleting collectors");

	for (EntityCollector* collector : _collectors)
		delete collector;

    _all_entities.clear();
    _new_entities.clear();
    _entities_to_delete.clear();
    _collectors.clear();
}

void EntityManager::delete_entity(Entity* entity)
{
    ECS_OUTPUT_DEBUG("Deletion of " << *entity << " was requested");
    auto it = std::find(_new_entities.begin(), _new_entities.end(), entity);
    if (it != _new_entities.end()) {
        ECS_OUTPUT_DEBUG("Entity was not registered yet. Can just delete it again");
        _new_entities.erase(it);
        do_delete_entity(entity, EntityDeletionContext::NeverAdded);
    } else {
        ECS_OUTPUT_DEBUG("Entity was already registered, queuing to delete it next frame");
        _entities_to_delete.push_back(entity);
        vector_erase_fast(_all_entities, entity);
    }
}

void EntityManager::do_delete_entity(Entity* entity, EntityDeletionContext context)
{
    ECS_OUTPUT_DEBUG("Deleting " << *entity << " in context " << context);
    switch (context) {
    case EntityDeletionContext::NeverAdded:
        break;
    case EntityDeletionContext::InUpdate:
        break;
    case EntityDeletionContext::OnDeleteCascade:
        vector_erase_fast_if_present(_all_entities, entity);
        vector_erase_fast_if_present(_new_entities, entity);
        vector_erase_fast_if_present(_entities_to_delete, entity);
        break;
    }

    vector_erase_fast_if_present(_entities_with_new_components, entity);

    for (EntityCollector* collector : _collectors)
        collector->remove_entity(entity);

    if (entity->has_component<TransformComponent>()) {
        // Deregister from parent
        TransformComponent& transform = entity->get_component<TransformComponent>();
        transform.unregister_from_parent();

        // Also delete all children
        ECS_OUTPUT_DEBUG("  -> Entity to delete has " << transform.get_children().size() << " children");
        for (Entity* child : transform.get_children()) {
            ECS_OUTPUT_DEBUG(" -> Deleting children " << *child);
            do_delete_entity(child, EntityDeletionContext::OnDeleteCascade);
        }
    }

    MemoryPool<Entity>::delete_object(entity);
}

void EntityManager::print_status()
{
    ECS_OUTPUT_DEBUG("\n\n---- Entity manager status ----");

    auto print_vec = [](const char* name, std::vector<Entity*> v) {
        ECS_OUTPUT_DEBUG("Contents of " << name << " (total: " << v.size() << ")");
        for (Entity* entity : v) {
            ECS_OUTPUT_DEBUG("   " << *entity << "");
        }
    };

    print_vec("_all_entities", _all_entities);
    print_vec("_entities_to_delete", _entities_to_delete);
    print_vec("_new_entities", _new_entities);
    print_vec("_entities_with_new_components", _new_entities);


    ECS_OUTPUT_DEBUG("\nCollectors (total: " << _collectors.size() << "):");

    for (EntityCollector* coll : _collectors) {
        coll->print_status();
    }

    ECS_OUTPUT_DEBUG("\n----------------------------------\n\n");
}

void EntityManager::on_component_added(Entity* entity)
{
    if (std::find(_entities_with_new_components.begin(), _entities_with_new_components.end(), entity) == _entities_with_new_components.end()) {
        _entities_with_new_components.push_back(entity);
    }
}
