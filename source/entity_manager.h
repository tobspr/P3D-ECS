#pragma once
#ifndef P3D_ENTITY_MANAGER_H
#define P3D_ENTITY_MANAGER_H

#include "config_module.h"

#include "component.h"
#include "entity.h"
#include "entity_collector.h"

#include <unordered_map>
#include <vector>

class EntityManager
{
  friend class Entity;

public:
  Entity *new_entity();

  template < typename T >
  T* new_system() {
    // Some stuff will happen here later on ..
    return new T(this);
  };

#ifndef INTERROGATE
  template < typename... Args>
  EntityCollector* new_collector() {

    static_assert(sizeof...(Args) > 0, "Cannot create a collector without component types!");

    EntityCollector* collector = EntityCollector::create<Args...>();
    // Todo: find collectors with same masks, and connect to them to avoid computing everything twice
    // For example, if collector1 collects Transform and Physics Components, and collector2 collects only Physics Components,
    // collector2 could start with the entities that collector1 collected, instead of having to iterate over all entities.
    _collectors.push_back(collector);

    ECS_OUTPUT_DEBUG("Registering new collector at " << collector);
    return collector; 
  }
#endif

  void single_step(float dt);

private:
  void register_entity(Entity* entity);


private:

  std::vector<Entity *> _new_entities;
  std::vector<EntityCollector*> _collectors;
};

#include "entity_manager.I"

#endif