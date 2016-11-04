
#include "entity.h"
#include "entity_manager.h"
#include "perf_utility.h"

#include <algorithm>

Entity::id_t Entity::next_id = 1000u;

Entity::~Entity() {
  // TODO: Add check if we were manually deleted and print an error in that case
  ECS_ON_DELETE("Entity");
  for (auto it : _components) {
    it.second->deleter();
  }
}

void Entity::on_component_added(Component::id_t id, Component *ptr) {
  if (_registered) {
    // In case the entity is already registered, reregister it to make
    // sure it is registered on all collectors
    _manager->on_component_added(this);
  }
}

void Entity::on_component_removed(Component::id_t id) {
  if (_registered) {
    // In case the entity is already registered, reregister it to make
    // sure it is registered correctly on all collectors
    _manager->on_component_removed(this);
  }
}

void Entity::on_registered_by_manager() { _registered = true; }

void Entity::remove() {
  if (_flagged_for_deletion) {
    // TODO: Warning about this entity being deleted twice (instead of printing to cerr)
    std::cerr << "Warning: remove() called twice." << std::endl;
    return;
  }
  _manager->delete_entity(this);
  _flagged_for_deletion = true;
}

void Entity::on_registered_to_collector(EntityCollector *collector) {
  if (!is_registered_to_collector(collector))
    _registered_collectors.push_back(collector);
}

bool Entity::is_registered_to_collector(EntityCollector *collector) {
  return vector_contains(_registered_collectors, collector);
}

void Entity::on_deregistered_from_collector(EntityCollector *collector) {
  vector_erase_fast(_registered_collectors, collector);
}

void Entity::on_ref_created(EntityRef* ref) {
  assert(!vector_contains(_referencing_refs, ref)); // How can this even happen?
  _referencing_refs.push_back(ref);
}

void Entity::on_ref_deleted(EntityRef* ref) {
  assert(vector_contains(_referencing_refs, ref)); // How can this even happen?
  vector_erase_fast(_referencing_refs, ref);
}


