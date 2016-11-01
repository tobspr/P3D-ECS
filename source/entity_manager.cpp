
#include "entity_manager.h"

void EntityManager::EntityPool::register_entity(Entity *ptr) {
    ECS_OUTPUT_DEBUG("Registered entity " << ptr << " (id: " << ptr->get_id() << ")");
    _entities.emplace(ptr->get_id(), ptr);
}

void EntityManager::register_entity_with_component(Component::id_t id, Entity *ptr)
{
    ECS_OUTPUT_DEBUG("New entity (" << ptr << ", id=" << ptr->get_id() << ") with component " << id << " registered.");
    get_component_pool(id).register_entity(ptr);    
}

Entity* EntityManager::new_entity()
{
    Entity* entity = new Entity(this);
    _new_entities.push_back(entity);
    ECS_OUTPUT_DEBUG("Constructed new entity with id " << entity->get_id());
    return entity;
}