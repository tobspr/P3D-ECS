
#include "entity_manager.h"
#include "perf_utility.h"
#include "memory_pool.h"
#include "output.h"
#include "fast_entity_map.h"
#include "leak_detector.h"
#include <algorithm>

EntityManager::EntityManager() {
  // Typical amount of entities
  ECS_ON_CREATE("EntityManager");
  size_t expected_entities = 100000;
  _uuid_to_entity = new FastEntityMap(expected_entities / 10);
  _all_entities.reserve(expected_entities);
  _entities_to_delete.reserve(expected_entities / 100);
  _new_entities.reserve(expected_entities / 3);
}

EntityManager::~EntityManager() {
  ECS_ON_DELETE("EntityManager");
  reset();

  for (EntityCollector* collector : _collectors)
    delete collector;

  delete _uuid_to_entity;
}

void
EntityManager::register_entity(Entity* entity) {
  entity->on_registered_by_manager();
  for (EntityCollector* collector : _collectors) {
    collector->consider_register(entity);
  }
}

Entity*
EntityManager::new_entity() {
  Entity* entity = do_create_entity(UUID::generate());
  _new_entities.push_back(entity);
  return entity;
}

Entity*
EntityManager::do_create_entity(UUID&& uuid) {
  Entity* entity = MemoryPool<Entity>::new_object(this, std::move(uuid));
  _uuid_to_entity->insert(entity);
  return entity;
}

void
EntityManager::process_changes() {

  // Add new entities - it is important that this comes first, so we can fillin
  // our pointers before anything gets deleted
  // TODO: During this iteration, an entity might be present in two vectors:
  // _all_entities and _new_entities. This is bad, since we always want only
  // one membership. We have to make a copy of the new entities
  for (Entity* entity : _new_entities) {
    register_entity(entity);
    if (entity->get_needs_fillin()) {
      entity->fillin_ptrs();
    }
    _all_entities.push_back(entity);
  }

  // Now that all entities were created, call the deserialized callback on them
  for (Entity* entity : _new_entities) {
    entity->on_deserialization_finished();
  }

  _new_entities.clear();

  // Delete queued entities
  while (!_entities_to_delete.empty()) {
    Entity* entity = _entities_to_delete.back();
    _entities_to_delete.pop_back();
    do_delete_entity(entity);
  }

  // Reevaluate changed entities
  for (Entity* entity : _entities_with_new_or_deleted_components) {

    // First, evaluate the current membership on the collectors
    std::vector<EntityCollector*> collectors_to_deregister_from;
    for (EntityCollector* collector : entity->get_collectors()) {
      if (!collector->entity_fits(entity)) {
        collectors_to_deregister_from.push_back(collector);
      }
    }

    // Deregister from all collectors which are now obsolete
    for (EntityCollector* collector : collectors_to_deregister_from) {
      collector->remove_entity(entity);
    }

    // Now register to all new collectors
    register_entity(entity);
  }
}

void
EntityManager::reset() {
  for (Entity* entity : _all_entities)
    MemoryPool<Entity>::delete_object(entity);

  for (Entity* entity : _new_entities)
    MemoryPool<Entity>::delete_object(entity);

  for (Entity* entity : _entities_to_delete)
    MemoryPool<Entity>::delete_object(entity);

  for (EntityCollector* collector : _collectors)
    collector->reset();

  _all_entities.clear();
  _new_entities.clear();
  _entities_to_delete.clear();
  _entities_with_new_or_deleted_components.clear();
}

void
EntityManager::delete_entity(Entity* entity) {
  // This method should only be called once
  ECS_ASSERT(!entity->is_flagged_for_deletion());

  auto it = ::p3d_ecs::performance::vector_find(_new_entities, entity);
  if (it != _new_entities.end()) {
    // Entity has never been registered, so we can instantly delete it.
    // TODO: This might have issues, since it differes from the behaviour when
    // the entity was registered. When it was registered, it will still be somewhat
    // valid (at least the pointer, although the entity will be empty) until the
    // next process changes. However, an unregistered entity will instantly be
    // deleted - Any pointer to it will be invalidated. So, if for example in a
    // system an entity is created, and then deleted, the pointer instantly is
    // unavailable. If, in contrast, an already registered entity is deleted,
    // it will stay valid until the end of the system iteration.
    ::p3d_ecs::performance::vector_erase(_new_entities, it);
    do_delete_entity(entity);
  } else {
    _entities_to_delete.push_back(entity);
    ::p3d_ecs::performance::vector_erase(_all_entities, entity);
  }
}

void
EntityManager::do_delete_entity(Entity* entity) {
  // When this method is called, we assume that the entity already has been
  // removed from the appropriate list.
  ::p3d_ecs::performance::vector_erase_if_contains(_entities_with_new_or_deleted_components,
                                                   entity);

  for (EntityCollector* collector : _collectors)
    collector->consider_remove_entity(entity);

  // Remove from UUID map
  _uuid_to_entity->remove(entity);

  MemoryPool<Entity>::delete_object(entity);
}

void
EntityManager::print_status() {

  auto print_vec = [](const char* name, std::vector<Entity*> v) {
    std::cout << "Contents of " << name << " (total: " << v.size() << ")" << std::endl;
    for (Entity* entity : v) {
      std::cout << "   " << *entity << std::endl;
    }
  };

  print_vec("_all_entities", _all_entities);
  print_vec("_entities_to_delete", _entities_to_delete);
  print_vec("_new_entities", _new_entities);
  print_vec("_entities_with_new_or_deleted_components", _new_entities);

  std::cout << "\nCollectors (total: " << _collectors.size() << "):" << std::endl;

  for (EntityCollector* coll : _collectors) {
    coll->print_status();
  }

  std::cout << "\n----------------------------------\n\n\n";
}

void
EntityManager::on_component_added(Entity* entity) {
  if (!::p3d_ecs::performance::vector_contains(_entities_with_new_or_deleted_components, entity)) {
    _entities_with_new_or_deleted_components.push_back(entity);
  }
}

void
EntityManager::on_component_removed(Entity* entity) {
  if (!::p3d_ecs::performance::vector_contains(_entities_with_new_or_deleted_components, entity)) {
    _entities_with_new_or_deleted_components.push_back(entity);
  }
}

Entity*
EntityManager::find_entity(Entity::id_t id) {
  // Search in new entities first, its more likely to find the entity there
  for (Entity* entity : _new_entities)
    if (entity->get_id() == id)
      return entity;
  for (Entity* entity : _all_entities)
    if (entity->get_id() == id)
      return entity;
  ECS_EMIT_WARNING(EM_0001, "Failed to find entity with id " << id);
  return nullptr;
}

Entity*
EntityManager::deserialize_entity(p3d_ecs::proto::Entity* message) {

  if (!UUID::is_valid_uuid(message->uuid())) {
    ECS_EMIT_WARNING(
      EM_0002, "Failed to deserialize message, required field 'uuid' not present or invalid!");
    // TODO: Maybe return an empty entity (new_entity()) instead?
    return nullptr;
  }

  UUID uuid = UUID::generate_from_string(message->uuid());
  // TODO: Require entity uuid to be unique, e.g. _uuid_to_entity->find(uuid) == nullptr

  Entity* entity = do_create_entity(std::move(uuid));
  entity->deserialize(message);
  _new_entities.push_back(entity);
  return entity;
}

Entity*
EntityManager::find_entity(const UUID& uuid) {
  // Search in new entities first, its more likely to find the entity there
  Entity* entity = _uuid_to_entity->find(uuid);
  if (entity == nullptr) {
    ECS_EMIT_WARNING(EM_0005, "Failed to find entity with uuid " << uuid.c_str());
    return nullptr;
  }

  // We should never be able to refer to a deleted entity during
  // serialization, so we don't have to search for deleted entities. However
  // it cannot hurt to also check this
  if (entity->is_flagged_for_deletion()) {
    ECS_EMIT_WARNING(EM_0004, "Referring by uuid to entity flagged for deletion ("
                                << *entity << ")! This might cause undefined behaviour.");
  }
  return entity;
}

size_t
EntityManager::get_memory_usage_bytes() const {
  size_t count = 0;
  count += _new_entities.capacity() * sizeof(Entity*);
  count += _entities_to_delete.capacity() * sizeof(Entity*);
  count += _all_entities.capacity() * sizeof(Entity*);
  count += _collectors.capacity() * sizeof(EntityCollector*);
  count += _entities_with_new_or_deleted_components.capacity() * sizeof(Entity*);
  count += _uuid_to_entity->get_memory_usage_bytes(get_num_entities());
  count += sizeof(EntityManager);
  return count;
}
