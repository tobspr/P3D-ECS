
#include "config_module.h"
#include "entity.h"
#include "entity_manager.h"
#include "perf_utility.h"
#include "all_components_gen.h"
#include "output.h"

#include "entity_ref.h"
#include "leak_detector.h"

#include <algorithm>

Entity::id_t Entity::next_id = 1000u;
const Entity::id_t Entity::EMPTY_ID = 0u;

Entity::Entity(EntityManager* manager, UUID&& uuid)
  : _manager(manager)
  , _component_mask(0u)
  , _registered(false)
  , _flagged_for_deletion(false)
  , _needs_fillin(false)
  , _uuid(std::move(uuid)) {
  ECS_ON_CREATE("Entity");
  // TODO: Add check if out of capacity
  _id = ++next_id;
}

Entity::~Entity() {
  // TODO: Add check if we were manually deleted and print an error in that case
  ECS_ON_DELETE("Entity");
  for (auto it : _components) {
    it.second->deleter();
  }
  for (EntityRef* entity_ref : _referencing_refs)
    entity_ref->on_entity_removed();
}

void
Entity::on_registered_by_manager() {
  _registered = true;
}

void
Entity::remove() {
  if (_flagged_for_deletion) {
    ECS_EMIT_WARNING(ENT_0001, "Entity::remove() called twice.");
    return;
  }

  _component_mask = 0u;
  for (component_pair_t& component : _components) {
    // TODO: We can probable merge both functions.
    component.second->on_removed();
    component.second->deleter();
  }
  _components.clear();

  // TODO: Evaluate if we should allow this or not
  // if (_needs_fillin) {
  //   ECS_EMIT_WARNING("Cannot delete entity! It is still waiting for fillin.\n");
  //   return;
  // }

  _manager->delete_entity(this);
  _flagged_for_deletion = true;

  // Since we just erased all components, we don't need to get filled in too.
  _needs_fillin = false;
}

void
Entity::on_registered_to_collector(EntityCollector* collector) {
  if (!is_registered_to_collector(collector))
    _registered_collectors.push_back(collector);
}

bool
Entity::is_registered_to_collector(EntityCollector* collector) {
  return ::p3d_ecs::performance::vector_contains(_registered_collectors, collector);
}

void
Entity::on_deregistered_from_collector(EntityCollector* collector) {
  ::p3d_ecs::performance::vector_erase(_registered_collectors, collector);
}

void
Entity::on_ref_created(EntityRef* ref) {
  ECS_ASSERT(
    !::p3d_ecs::performance::vector_contains(_referencing_refs, ref)); // How can this even happen?
  _referencing_refs.push_back(ref);
}

void
Entity::on_ref_deleted(EntityRef* ref) {
  ECS_ASSERT(
    ::p3d_ecs::performance::vector_contains(_referencing_refs, ref)); // How can this even happen?
  ::p3d_ecs::performance::vector_erase(_referencing_refs, ref);
}

p3d_ecs::proto::Entity*
Entity::serialize() const {
  p3d_ecs::proto::Entity* entity_proto = new p3d_ecs::proto::Entity;
  entity_proto->set_uuid(_uuid.c_str(), UUID::uuid_length);

  p3d_ecs::proto::Components* components = entity_proto->mutable_components();
  for (const component_pair_t& component : _components) {
    component.second->serialize(components);
  }
  return entity_proto;
}

void
Entity::deserialize(p3d_ecs::proto::Entity* message) {
  if (!message->has_components()) {
    ECS_EMIT_WARNING(ENT_0002, "No components definition found in entity message!");
    return;
  }

  _needs_fillin = true;
  const p3d_ecs::proto::Components& components = message->components();
  deserialize_components(this, components);
}

void
Entity::fillin_ptrs() {
  for (const component_pair_t& component : _components)
    component.second->fillin_ptrs();
  _needs_fillin = false;
}

void
Entity::on_deserialization_finished() {
  for (const component_pair_t& component : _components)
    component.second->on_deserialization_finished();
}

bool
Entity::data_equals(const Entity* other) const {
  if (other->_component_mask != _component_mask)
    return false;
  if (this == other)
    return true;
  for (const component_pair_t& component : _components) {
    const Component& other_component = other->get_component_by_id(component.first);
    if (!component.second->data_equals(other_component))
      return false;
  }

  return true;
}

Component&
Entity::get_component_by_id(Component::id_t id) {
  ECS_ASSERT(has_component_by_id(id));
  // ECS_ASSERT(!_flagged_for_deletion); // TODO: Might be problematic, should maybe
  // allow this
  for (component_pair_t& id_and_ptr : _components) {
    if (id_and_ptr.first == id)
      return *static_cast<Component*>(id_and_ptr.second);
  }

  ECS_ASSERT(false); // should never happen, the first assert will trigger instead
                     // of this one
  return *static_cast<Component*>(nullptr);
}

const Component&
Entity::get_component_by_id(Component::id_t id) const {
  return const_cast<Entity*>(this)->get_component_by_id(id);
}

void
Entity::remove_component_by_id(Component::id_t id) {
  ECS_ASSERT(!_flagged_for_deletion); // Might be problematic
  ECS_ASSERT(has_component_by_id(id));

  _component_mask &= ~Component::to_bitmask(id);
  auto it = _components.begin();
  for (; it != _components.end(); ++it) {
    if (it->first == id) {
      // TODO: We can probable merge both functions.
      it->second->on_removed();
      it->second->deleter();
      ::p3d_ecs::performance::vector_erase(_components, it);
      break;
    }
  }
  if (_registered) {
    // In case the entity is already registered, reregister it to make
    // sure it is registered correctly on all collectors
    _manager->on_component_removed(this);
  }
}

void
Entity::add_component_by_id_and_ptr(Component::id_t id, Component* ptr) {
  _component_mask |= Component::to_bitmask(id);
  _components.emplace_back(id, ptr);

  if (_registered) {
    // In case the entity is already registered, reregister it to make
    // sure it is registered on all collectors
    _manager->on_component_added(this);
  }
}