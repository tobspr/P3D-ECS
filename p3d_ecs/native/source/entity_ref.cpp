
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

  // This is problematic, and it does not harm not to check this.
  // While one can access entities flagged for deletion, they will
  // get unavailable soon anyways:
  //   if (_cached_ptr->is_flagged_for_deletion())
  //     return false;
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
    // a_ssert(_ref_id == Entity::EMPTY_ID)
    // ^ This can actually happen, when the ref points to an entity, but has not
    //   accquired a pointer yet. In this case, the pointer is NULL but id is
    //   not empty. (Detected by Entity Ref Test - CASE 85)
    _ref_id = Entity::EMPTY_ID;
  } else {
    _cached_ptr->on_ref_deleted(this);
    _cached_ptr = nullptr;
    _ref_id = Entity::EMPTY_ID;
  }
}

void EntityRef::reset(Entity *new_ptr) {
  if (new_ptr == _cached_ptr) {
    if (new_ptr == nullptr) {
      // We do not have an actual ref to our entity right now.
      // Also, it seems that the user wants to reset this reference to empty.
      // so just set the id to empty and were done, no de-registration required.
      _ref_id = Entity::EMPTY_ID;
    }
    return;
  }
  reset();
  if (new_ptr == nullptr)
    return;
  _cached_ptr = new_ptr;
  _cached_ptr->on_ref_created(this);
  _ref_id = _cached_ptr->get_id();
}

void EntityRef::reset(Entity::id_t id) {
  if (id == _ref_id)
    return; // Nothing to do

  if (_cached_ptr != nullptr) {
    // Deregister from old pointer (Detected by Entity Ref Test - CASE 155)
    _cached_ptr->on_ref_deleted(this);
  }

  _cached_ptr = nullptr;
  _ref_id = id;
}
