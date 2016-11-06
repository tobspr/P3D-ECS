#pragma once

#ifndef P3D_ECS_ENTITY_REF_H
#define P3D_ECS_ENTITY_REF_H

#include "config_module.h"

class Entity;
class EntityManager;

// Forward declare, see entity.h (Entity::id_t)
// Avoids having to include entity.h
using Entity_id_t = uint_fast64_t;

// Like weak_ptr<Entity> in fast.
// Usage:
//
//   EntityRef my_ref;
//   if (my_ref.has_access())
//       my_ref->do_something();
//
class EntityRef final {
  friend class Entity;

public:
  EntityRef();
  explicit EntityRef(Entity_id_t id);
  explicit EntityRef(Entity* entity);
  ~EntityRef();

  inline Entity_id_t get_id() const { return _ref_id; }
  Entity* get_ptr(EntityManager* mgr);

  bool is_empty() const;

  bool has_access() const;
  bool fill_ptr(EntityManager* mgr);

  inline Entity* operator->() { return _cached_ptr; }
  inline const Entity* operator->() const { return _cached_ptr; }

  // Prevent copies, this would be bad because the ref registers on the
  // entity
  EntityRef(const EntityRef& other) = delete;
  EntityRef(EntityRef&& other) = delete;
  EntityRef& operator=(const EntityRef& other) = delete;
  EntityRef& operator=(EntityRef&& other) = delete;

  EntityRef& operator=(Entity* entity);

  operator Entity*();

  void reset();
  void reset(Entity* new_ptr);
  void reset(Entity_id_t id);

private:
  void on_entity_removed();

  Entity* _cached_ptr;
  Entity_id_t _ref_id;
};

#endif
