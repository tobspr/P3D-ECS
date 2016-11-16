
#include "output.h"

template <typename T>
ECS_FORCEINLINE T&
Entity::get_component() {
  Component::id_t id = Component::extract_id<T>();
  return static_cast<T&>(get_component_by_id(id));
};

template <typename T>
ECS_FORCEINLINE const T&
Entity::get_component() const {
  return const_cast<Entity*>(this)->get_component<T>();
};

template <typename T>
ECS_FORCEINLINE bool
Entity::has_component() const {
  return has_component_by_id(Component::extract_id<T>());
};

ECS_FORCEINLINE bool
Entity::has_component_by_id(Component::id_t id) const {
  return (_component_mask & Component::to_bitmask(id)) != 0u;
}

template <typename T>
void
Entity::remove_component() {
  remove_component_by_id(Component::extract_id<T>());
}

#ifndef INTERROGATE
template <typename T>
T&
Entity::new_component() {
  if (has_component<T>()) {
    ECS_EMIT_WARNING(ENT_0004,
                     "Adding component twice! Returning existing component without changes.");
    return get_component<T>();
  }
  if (_flagged_for_deletion) {
    ECS_EMIT_WARNING(ENT_0005,
                     "Adding a component to an entity which is already flagged for deletion!");
  }

  T* component = MemoryPool<T>::new_object(this);
  add_component_by_id_and_ptr(Component::extract_id<T>(), component);
  return *component;
};

#endif