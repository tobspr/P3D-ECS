
// AUTOGENERATED. DO NOT EDIT!
// Instead modify the component meta file.

#include "physics_component_meta.h"

#include "plain_text_serializer.h"

IMPLEMENT_COMPONENT_BASE(PhysicsComponent, PhysicsComponentMeta, 1u);

// AUTOGEN: member names for plain text serialization
const char* PhysicsComponentMeta::is_static_CSTR = "is_static";
const char* PhysicsComponentMeta::mass_CSTR = "mass";

void
PhysicsComponentMeta::serialize(PlainTextSerializer* serializer) const {
  if (!(_is_static == bool(false))) {
    serializer->serialize_prop(is_static_CSTR, _is_static);
  }
  if (!(compare_float(_mass, 10.0))) {
    serializer->serialize_prop(mass_CSTR, _mass);
  }
}

bool
PhysicsComponentMeta::data_equals(const Component& other) const {
  if (&other == this)
    return true; // same instance

  const PhysicsComponentMeta* downcasted_ptr =
    static_cast<const PhysicsComponentMeta*>(&other); // There must be a better way than this
  assert(downcasted_ptr != nullptr); // Should never happen! Since our component table would be corrupted then.
  const PhysicsComponentMeta& other_ref = *downcasted_ptr;

  if (!(_is_static == other_ref._is_static))
    return false;
  if (!(compare_float(_mass, other_ref._mass)))
    return false;
  return true;
}