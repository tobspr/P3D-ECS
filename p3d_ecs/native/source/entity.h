#pragma once
#ifndef P3DECS_ENTITY_H
#define P3DECS_ENTITY_H

#include "config_module.h"
#include "component.h"
#include "uuid.h"

#ifndef INTERROGATE
#include "entity.pb.h"
#endif

#include <stdint.h>
#include <cassert>
#include <utility>
#include <iostream>

template <typename T>
class MemoryPool;
class EntityManager;
class EntityCollector;
class EntityRef;
class PlainTextSerializer;

class Entity final {
  friend class EntityManager;
  friend class EntityCollector;
  friend class MemoryPool<Entity>;
  friend class EntityRef;

public:
  using id_t = uint_fast64_t;
  using component_pair_t = std::pair<Component::id_t, Component*>;

private:
  static id_t next_id;

public:
  static const id_t EMPTY_ID;

  // Required for unittests
  static inline void reset_id_pool() { next_id = 1000u; };

  ~Entity();
  Entity(const Entity& other) = delete;            // nope
  Entity(Entity&& other) = delete;                 // nope
  Entity& operator=(const Entity& other) = delete; // nope
  Entity& operator=(Entity&& other) = delete;      // nope

  ECS_FORCEINLINE id_t get_id() const { return _id; };
  ECS_FORCEINLINE const UUID& get_uuid() const { return _uuid; };
  ECS_FORCEINLINE EntityManager* get_manager() const { return _manager; };
  ECS_FORCEINLINE bool get_needs_fillin() const { return _needs_fillin; }

  ECS_FORCEINLINE size_t get_num_components() const { return _components.size(); };
  ECS_FORCEINLINE Component::bitmask_t get_component_mask() const { return _component_mask; }
  ECS_FORCEINLINE bool is_flagged_for_deletion() const { return _flagged_for_deletion; }

  Component& get_component_by_id(Component::id_t id);
  const Component& get_component_by_id(Component::id_t id) const;
  ECS_FORCEINLINE bool has_component_by_id(Component::id_t id) const;
  void remove_component_by_id(Component::id_t id);

  template <typename T>
  ECS_FORCEINLINE T& get_component();

  template <typename T>
  ECS_FORCEINLINE const T& get_component() const;

  template <typename T>
  ECS_FORCEINLINE bool has_component() const;

  template <typename T>
  inline void remove_component();

  template <typename T>
  inline T& new_component();

  void remove();
#ifndef INTERROGATE
  p3d_ecs::proto::Entity* serialize() const;
  void deserialize(p3d_ecs::proto::Entity* message);
#endif

  bool data_equals(const Entity* other) const;
  void fillin_ptrs();
  void on_deserialization_finished();

private: // Internal Interface
  explicit Entity(EntityManager* manager, UUID&& uuid);

  void on_registered_by_manager();

  void add_component_by_id_and_ptr(Component::id_t id, Component* ptr);

  bool is_registered_to_collector(EntityCollector* collector);
  void on_registered_to_collector(EntityCollector* collector);
  void on_deregistered_from_collector(EntityCollector* collector);

  void on_ref_created(EntityRef* ref);
  void on_ref_deleted(EntityRef* ref);

  ECS_FORCEINLINE const std::vector<EntityCollector*>& get_collectors() const {
    return _registered_collectors;
  };

private: // Private stuff
  EntityManager* _manager;

  bool _registered;
  bool _needs_fillin;
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
