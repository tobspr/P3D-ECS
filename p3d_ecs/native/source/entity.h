#pragma once
#ifndef P3DECS_ENTITY_H
#define P3DECS_ENTITY_H

#include "config_module.h"
#include "component.h"
#include "memory_pool.h"
#include "entity_ref.h"
#include "uuid.h"

#include <stdint.h>
#include <cassert>
#include <utility>
#include <iostream>

class EntityManager;
class EntityCollector;
class EntityRef;
class PlainTextSerializer;

class Entity final {
  friend class EntityManager;
  friend class EntityCollector;
  friend class MemoryPool<Entity>;
  friend class EntityRef;

  using id_t = uint_fast64_t;
  using component_pair_t = std::pair<Component::id_t, Component*>;
  static id_t next_id;

public:
  static const id_t EMPTY_ID = 0u;

  // Required for unittests
  static inline void reset_id_pool() { next_id = 1000u; };

  ECS_FORCEINLINE ~Entity();
  Entity(const Entity& other) = delete;            // nope
  Entity(Entity&& other) = delete;                 // nope
  Entity& operator=(const Entity& other) = delete; // nope
  Entity& operator=(Entity&& other) = delete;      // nope

  ECS_FORCEINLINE id_t get_id() const { return _id; };
  ECS_FORCEINLINE const UUID& get_uuid() const { return _uuid; };

  ECS_FORCEINLINE size_t get_num_components() const { return _components.size(); };
  ECS_FORCEINLINE Component::bitmask_t get_component_mask() const { return _component_mask; }
  ECS_FORCEINLINE bool is_flagged_for_deletion() const { return _flagged_for_deletion; }

  ECS_FORCEINLINE Component& get_component_by_id(Component::id_t id);
  ECS_FORCEINLINE const Component& get_component_by_id(Component::id_t id) const;
  ECS_FORCEINLINE bool has_component_by_id(Component::id_t id) const;

  template <typename T>
  ECS_FORCEINLINE T& get_component();

  template <typename T>
  ECS_FORCEINLINE const T& get_component() const;

  template <typename T>
  ECS_FORCEINLINE bool has_component() const;

  template <typename T>
  inline void remove_component();

  template <typename T, typename... Args>
  inline T& new_component(Args&&... args);

  void serialize(PlainTextSerializer* serializer) const;
  void remove();

  bool data_equals(const Entity* other) const;

private: // Internal Interface
  ECS_FORCEINLINE explicit Entity(EntityManager* manager, UUID&& uuid);

  void on_registered_by_manager();
  void on_component_added(Component::id_t id, Component* ptr);
  void on_component_removed(Component::id_t id);

  bool is_registered_to_collector(EntityCollector* collector);
  void on_registered_to_collector(EntityCollector* collector);
  void on_deregistered_from_collector(EntityCollector* collector);

  void on_ref_created(EntityRef* ref);
  void on_ref_deleted(EntityRef* ref);

  ECS_FORCEINLINE const std::vector<EntityCollector*>& get_collectors() const { return _registered_collectors; };

private: // Private stuff
  EntityManager* _manager;

  bool _registered;
  bool _flagged_for_deletion;
  id_t _id;
  UUID _uuid;
  Component::bitmask_t _component_mask;

  std::vector<component_pair_t> _components;
  std::vector<EntityCollector*> _registered_collectors;
  std::vector<EntityRef*> _referencing_refs;
};

#include "entity.I"

inline ostream& operator<<(ostream& stream, const Entity& entity) {
  return stream << "[Entity @" << &entity << ", id=" << entity.get_id()
                << ", components=" << entity.get_num_components() << "]";
};

#endif
