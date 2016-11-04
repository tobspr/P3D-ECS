#pragma once
#ifndef P3D_ENTITY_MANAGER_H
#define P3D_ENTITY_MANAGER_H

#include "config_module.h"

#include "component.h"
#include "entity.h"
#include "entity_collector.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <iostream>

class EntityManager final {
  friend class Entity;

  enum class EntityDeletionContext { InUpdate, NeverAdded, OnDeleteCascade };

  friend inline std::ostream &
  operator<<(std::ostream &stream,
             EntityManager::EntityDeletionContext context);

public:
  EntityManager();
  ~EntityManager();

  Entity *new_entity();

  template <typename T> T *new_system() {
    // Can not create a new system after entities have been constructed already
    // TODO: Make this possible
    assert(_all_entities.empty() && _new_entities.empty() &&
           _entities_to_delete.empty());

    // Some stuff will happen here later on ..
    return new T(this);
  };

  template <typename... Args> EntityCollector *new_collector() {
    static_assert(sizeof...(Args) > 0,
                  "Cannot create a collector without component types!");

    EntityCollector *collector = EntityCollector::create<Args...>();
    // Todo: find collectors with same masks, and connect to them to avoid
    // computing everything twice
    // For example, if collector1 collects Transform and Physics Components, and
    // collector2 collects only Physics Components,
    // collector2 could start with the entities that collector1 collected,
    // instead of having to iterate over all entities.
    _collectors.push_back(collector);

    ECS_OUTPUT_SPAM("Registering new collector at " << collector);
    return collector;
  }

  void process_changes();
  void reset();

  void print_status();

  size_t get_num_entities() const {
    return _all_entities.size() + _new_entities.size() +
           _entities_to_delete.size();
  };

private:
  void register_entity(Entity *entity);
  void delete_entity(Entity *entity);

  void on_component_added(Entity *entity);
  void on_component_removed(Entity *entity);

  void do_delete_entity(Entity *entity, EntityDeletionContext context);

  const std::vector<EntityCollector *> &get_collectors() const {
    return _collectors;
  }

private:
  // Each entity can only belong to one of these 3 containers at a time:
  std::vector<Entity *> _new_entities;
  std::vector<Entity *> _entities_to_delete;
  std::vector<Entity *> _all_entities;

  std::vector<EntityCollector *> _collectors;

  std::vector<Entity *> _entities_with_new_or_deleted_components;
};

inline std::ostream &operator<<(std::ostream &stream,
                                EntityManager::EntityDeletionContext context) {
  switch (context) {
  case EntityManager::EntityDeletionContext::InUpdate:
    return stream << "[EDC::InUpdate]";
  case EntityManager::EntityDeletionContext::NeverAdded:
    return stream << "[EDC::NeverAdded]";
  case EntityManager::EntityDeletionContext::OnDeleteCascade:
    return stream << "[EDC::OnDeleteCascade]";
  }
  return stream << "[EDC:: Undefined!]";
}

#endif