#pragma once
#ifndef P3D_ENTITY_MANAGER_H
#define P3D_ENTITY_MANAGER_H

#include "config_module.h"

#include "component.h"
#include "entity.h"
#include "entity_collector.h"
#include "uuid.h"

#ifndef INTERROGATE
#include "entity.pb.h"
#endif

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <iostream>
#include <utility>
#include <functional>

class FastEntityMap;

class EntityManager final {
  friend class Entity;

public:
  EntityManager();
  ~EntityManager();

  Entity* new_entity();

  template <typename T>
  T* new_system() {
    // Can not create a new system after entities have been constructed already
    // TODO: Make this possible
    ECS_ASSERT(_all_entities.empty() && _new_entities.empty() && _entities_to_delete.empty());
    return new T(this);
  };

  template <typename... Args>
  EntityCollector* new_collector() {
    static_assert(sizeof...(Args) > 0, "Cannot create a collector without component types!");
    EntityCollector* collector = EntityCollector::create<Args...>();
    _collectors.push_back(collector);
    return collector;
  }

  void process_changes();
  void reset();
  void print_status();

  size_t get_num_entities() const {
    return _all_entities.size() + _new_entities.size() + _entities_to_delete.size();
  };

  Entity* find_entity(Entity::id_t id);
  Entity* find_entity(const UUID& uuid);

#ifndef INTERROGATE
  Entity* deserialize_entity(p3d_ecs::proto::Entity* message);
#endif

  size_t get_memory_usage_bytes() const;

private:
  void register_entity(Entity* entity);
  void delete_entity(Entity* entity);

  void on_component_added(Entity* entity);
  void on_component_removed(Entity* entity);

  void do_delete_entity(Entity* entity);

  const std::vector<EntityCollector*>& get_collectors() const { return _collectors; }

  using entity_ref_wrap_t = std::reference_wrapper<Entity*>;
  using entity_vec_ref_wrap_t = std::reference_wrapper<std::vector<Entity*>>;

private:
  Entity* do_create_entity(UUID&& uuid);

  // Each entity can only belong to one of these 3 containers at a time:
  std::vector<Entity*> _new_entities;       // Newly created entities
  std::vector<Entity*> _entities_to_delete; // Entities flagged for deletion
  std::vector<Entity*> _all_entities;       // Remaining

  // All collectors
  std::vector<EntityCollector*> _collectors;

  // List of entities which have new or deleted components since the last
  // call to process_changes()
  std::vector<Entity*> _entities_with_new_or_deleted_components;

  // Mapping of UUID's to entities for fast look_up. Pointer and not member to speed up compilation
  // speed.
  FastEntityMap* _uuid_to_entity;
};

#endif