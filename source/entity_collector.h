#pragma once

#ifndef P3D_ECS_ENTITYCOLLECTOR_H
#define P3D_ECS_ENTITYCOLLECTOR_H

#include "component.h"

#include <vector>
#include <unordered_set>
#include <initializer_list>

class Entity;

class EntityCollector {
  /**
   * The entity collector is a class which collects all entities which
   * have certain components. For example:
   *    EntityCollector::create<TransformComponent, PhysicsComponent>()
   * will create a collector which gives access to all entities with at least
   * a transform and physics component.
   */

  using component_list = std::vector<Component::id_t>;
  using entity_list = std::unordered_set<Entity*>;

  EntityCollector(const component_list& components);

 public:
#ifndef INTERROGATE
  template <typename... Components>
  static EntityCollector* create() {
    component_list components = {Component::extract_id<Components>()...};
    return new EntityCollector(components);
  };
#endif

  void consider_register(Entity* entity);

  inline const component_list& get_components() const { return _components; };
  inline Component::bitmask_t get_component_mask() const {
    return _component_mask;
  };

  inline entity_list::const_iterator begin() const {
    return _matching_entities.cbegin();
  };
  inline entity_list::const_iterator end() const {
    return _matching_entities.cend();
  };

  inline entity_list::iterator begin() { return _matching_entities.begin(); };
  inline entity_list::iterator end() { return _matching_entities.end(); };

  inline size_t size() const { return _matching_entities.size(); };

 private:
  component_list _components;
  entity_list _matching_entities;
  Component::bitmask_t _component_mask;
};

#endif