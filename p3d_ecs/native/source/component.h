#pragma once
#ifndef P3DECS_COMPONENT_H
#define P3DECS_COMPONENT_H

#include "config_module.h"

#include <stdint.h>

class PlainTextSerializer;
class Entity;
class EntityManager;

struct Component
{
  friend class EntityManager;
  friend class Entity;

  using id_t = uint_fast8_t;
  using bitmask_t = uint_fast64_t;

  static inline bitmask_t to_bitmask(id_t id) { return ((bitmask_t)1u) << id; }

  virtual void deleter() = 0;

  template <typename T>
  static inline id_t extract_id()
  {
    return T::component_id;
  }

  // Prevent copies / moves
  Component(const Component& other) = delete;
  Component(Component&& other) = delete;
  Component& operator=(const Component& other) = delete;
  Component& operator=(Component&& other) = delete;

  virtual void serialize(PlainTextSerializer* serializer) const = 0;
  virtual const char* get_class_name() const = 0;

protected:
  virtual bool data_equals(const Component& other) const = 0;

  Component(Entity* entity)
    : _entity(entity){};

  Entity* _entity;
};

#define DEFINE_COMPONENT(ClassName)                                            \
  friend class MemoryPool<ClassName>;                                          \
  friend class EntityManager;                                                  \
  friend class Entity;                                                         \
  virtual void deleter() override                                              \
  {                                                                            \
    MemoryPool<ClassName>::delete_object_from_upcast<Component>(this);         \
    this->~ClassName();                                                        \
  }                                                                            \
  virtual const char* get_class_name() const override { return class_name; };

#define DEFINE_COMPONENT_BASE()                                                \
  friend class Entity;                                                         \
  static const id_t component_id;                                              \
  static const char* class_name;

#define IMPLEMENT_COMPONENT_BASE(component_name, meta_name, id)                \
  const Component::id_t meta_name::component_id = id;                          \
  const char* meta_name::class_name = "" #component_name "";

#endif
