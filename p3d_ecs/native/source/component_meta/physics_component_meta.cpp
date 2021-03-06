
// AUTOGENERATED. DO NOT EDIT!
// Instead modify the component meta file.

#include "physics_component_meta.h"
#include "entity.h"
#include "serialization_helpers.h"
#include "entity_manager.h"

#include <functional>
IMPLEMENT_COMPONENT_BASE(PhysicsComponent, PhysicsComponentMeta, 2u);


void
PhysicsComponentMeta::serialize(p3d_ecs::proto::Components* dest) const {
  auto* proto_obj = dest->mutable_physics_component();

  if (!(::p3d_ecs::utility::float_equals(_mass, 10.0))) {
    proto_obj->mutable_mass()->set_value(_mass);
  }
  if (!(_is_static == bool(false))) {
    proto_obj->mutable_is_static()->set_value(_is_static);
  }
}

void
PhysicsComponentMeta::deserialize(const p3d_ecs::proto::PhysicsComponent& message) {


  // Member: mass
  if (!message.has_mass()) {
    _mass = 10.0;
  } else {
    _mass = message.mass().value();
  }

  // Member: is_static
  if (!message.has_is_static()) {
    _is_static = false;
  } else {
    _is_static = message.is_static().value();
  }

}

void
PhysicsComponentMeta::fillin_ptrs() {
  // No properties which require fillin detected.
}

bool
PhysicsComponentMeta::data_equals(const Component &other) const {
  // Notice: Its not required to check if we are comparing against ourself.
  // This is because data_equals can only be called from an entity, and
  // the entity already checks for pointer equalness.
  const PhysicsComponentMeta& other_ref = static_cast<const PhysicsComponentMeta&>(other);

  if (!(::p3d_ecs::utility::float_equals(_mass, other_ref._mass)))
    return false;
  if (!(_is_static == other_ref._is_static))
    return false;
  return true;
}

