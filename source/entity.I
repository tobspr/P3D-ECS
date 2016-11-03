
#include "all_components.h"

inline Entity::Entity(EntityManager *manager)
    : _manager(manager), _component_mask(0u), _registered(false),
      _flagged_for_deletion(false) {
  ECS_ON_CREATE("Entity");
  // TODO: Add check if out of capacity
  _id = ++next_id;
}

template <typename T> T &Entity::get_component() {
  assert(has_component<T>());
  // assert(!_flagged_for_deletion); // TODO: Might be problematic, should maybe
  // allow this
  Component::id_t id = Component::extract_id<T>();
  for (component_pair_t &id_and_ptr : _components) {
    if (id_and_ptr.first == id)
      return *static_cast<T *>(id_and_ptr.second);
  }

  assert(false); // should never happen, the first assert will trigger instead
                 // of this one
  return *static_cast<T *>(nullptr);
};

template <typename T> const T &Entity::get_component() const {
  return const_cast<Entity *>(this)->get_component<T>();
};

template <typename T> bool Entity::has_component() const {
  return (_component_mask &
          Component::to_bitmask(Component::extract_id<T>())) != 0u;
};

template <typename T> void Entity::remove_component() {
  assert(!_flagged_for_deletion); // Might be problematic
  assert(has_component<T>());
  if (std::is_same<T, TransformComponent>::value) {
    // Cannot remove transform component if it has still children
    // TODO: make this an interface, e.g. can_delete_component or so
    assert(get_component<TransformComponent>().get_children().empty());
  }
  _component_mask &= ~Component::to_bitmask(Component::extract_id<T>());

  Component::id_t id = Component::extract_id<T>();
  auto it = _components.begin();
  for (; it != _components.end(); ++it) {
    if (it->first == id) {
      it->second->deleter();
      vector_erase_fast(_components, it);
      break;
    }
  }
  on_component_removed(id);
}

#ifndef INTERROGATE
template <typename T, typename... Args>
T &Entity::new_component(Args &&... args) {
  assert(!_flagged_for_deletion);
  assert(!has_component<T>());

  T *component = MemoryPool<T>::new_object(this, std::forward<Args>(args)...);
  Component::id_t id = Component::extract_id<T>();

  _component_mask |= Component::to_bitmask(id);
  _components.emplace_back(id, component);

  on_component_added(id, component);
  return *component;
};

#endif