#pragma once
#ifndef P3DECS_ENTITY_H
#define P3DECS_ENTITY_H

#include "config_module.h"
#include "component.h"
#include "memory_pool.h"

#include <stdint.h>
#include <cassert>
#include <utility>
#include <iostream>

class EntityManager;
class EntityCollector;

class Entity final {
  friend class EntityManager;
  friend class EntityCollector;
  friend class MemoryPool<Entity>;

  using id_t = uint_fast64_t;
  using component_pair_t = std::pair<Component::id_t, Component *>;
  static id_t next_id;

public:
  // Required for unittests
  static inline void reset_id_pool() { next_id = 1000u; };

  ~Entity();
  Entity(const Entity &other) = delete;            // nope
  Entity(Entity &&other) = delete;                 // nope
  Entity &operator=(const Entity &other) = delete; // nope
  Entity &operator=(Entity &&other) = delete;      // nope

  inline id_t get_id() const { return _id; };
  inline size_t get_num_components() const { return _components.size(); };
  inline Component::bitmask_t get_component_mask() const {
    return _component_mask;
  }
  inline bool is_flagged_for_deletion() const { return _flagged_for_deletion; }

  template <typename T> inline T &get_component();

  template <typename T> inline const T &get_component() const;

  template <typename T> inline bool has_component() const;

  template <typename T> inline void remove_component();

  template <typename T, typename... Args>
  inline T &new_component(Args &&... args);

  void remove();

private:
  inline explicit Entity(EntityManager *manager);

  void on_registered_by_manager();
  void on_component_added(Component::id_t id, Component *ptr);
  void on_component_removed(Component::id_t id);

  bool is_registered_to_collector(EntityCollector *collector);
  void on_registered_to_collector(EntityCollector *collector);
  void on_deregistered_from_collector(EntityCollector *collector);

  inline const std::vector<EntityCollector *> &get_collectors() const {
    return _registered_collectors;
  };

  EntityManager *_manager;

  bool _registered;
  bool _flagged_for_deletion;
  id_t _id;
  Component::bitmask_t _component_mask;

  std::vector<component_pair_t> _components;
  std::vector<EntityCollector *> _registered_collectors;
};

#include "entity.I"

inline ostream &operator<<(ostream &stream, const Entity &entity) {
  return stream << "[Entity @" << &entity << ", id=" << entity.get_id()
                << ", components=" << entity.get_num_components() << "]";
};

#endif
