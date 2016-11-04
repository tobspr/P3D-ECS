
#include "entity_ref.h"
#include "entity_manager.h"
#include "entity.h"

EntityRef::EntityRef() : _ref_id(Entity::EMPTY_ID), _cached_ptr(nullptr) {}

EntityRef::EntityRef(Entity_id_t id) : _ref_id(id), _cached_ptr(nullptr) {}

EntityRef::EntityRef(Entity *entity) : _cached_ptr(entity) {
  if (entity != nullptr) {
    _ref_id = entity->get_id();
    entity->on_ref_created(this);
  } else {
    _ref_id = Entity::EMPTY_ID;
  }
}

EntityRef::~EntityRef() {
  if (_cached_ptr != nullptr)
    _cached_ptr->on_ref_deleted(this);
}

Entity *EntityRef::get_ptr(EntityManager *mgr) {
  if (!fill_ptr(mgr))
    return nullptr;
  return _cached_ptr;
}

bool EntityRef::has_access() const {
  if (_cached_ptr == nullptr)
    return false;
  // if (_cached_ptr->is_flagged_for_deletion())
  // return false;
  return true;
}

bool EntityRef::fill_ptr(EntityManager *mgr) {
  if (_ref_id == Entity::EMPTY_ID) {
    // This is fine, if the reference is not set, we still were successfull to
    // fill the pointer
    assert(_cached_ptr == nullptr);
    return true;
  }
  if (_cached_ptr != nullptr) {
    assert(_cached_ptr->get_id() == _ref_id);
    return true;
  } else {

    _cached_ptr = mgr->find_entity(_ref_id);
    assert(
        _cached_ptr !=
        nullptr); // Entity pointed to is no longer valid - how can this happen?
    assert(_cached_ptr->get_id() == _ref_id); // should never happen
    _cached_ptr->on_ref_created(this);
    return true;
  }
}

EntityRef &EntityRef::operator=(Entity *entity) {
  reset(entity);
  return *this;
}

bool EntityRef::is_empty() const { return _ref_id == Entity::EMPTY_ID; }

void EntityRef::on_entity_removed() {
  _ref_id = Entity::EMPTY_ID;
  _cached_ptr = nullptr;
}

EntityRef::operator Entity *() { return _cached_ptr; }

void EntityRef::reset() {
  if (_cached_ptr == nullptr) {
    assert(_ref_id == Entity::EMPTY_ID); // Cannot hurt to check
  } else {
    _cached_ptr->on_ref_deleted(this);
    _cached_ptr = nullptr;
    _ref_id = Entity::EMPTY_ID;
  }
}

void EntityRef::reset(Entity *new_ptr) {
  reset();
  if (new_ptr == nullptr)
    return;
  _cached_ptr = new_ptr;
  _ref_id = _cached_ptr->get_id();
}
