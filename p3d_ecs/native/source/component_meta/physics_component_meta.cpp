
// AUTOGENERATED. DO NOT EDIT!
// Instead modify the component meta file.

#include "physics_component_meta.h"

#include "plain_text_serializer.h"

IMPLEMENT_COMPONENT_BASE(PhysicsComponent, PhysicsComponentMeta, 1u);

// AUTOGEN: member names for plain text serialization
const char* PhysicsComponentMeta::is_static_CSTR = "is_static";
const char* PhysicsComponentMeta::mass_CSTR = "mass";

void PhysicsComponentMeta::serialize(PlainTextSerializer* serializer) const {
  // Serialize is_static
  if (_is_static != false) {
      serializer->serialize_prop(is_static_CSTR, _is_static);
  }
  // Serialize mass
  if (std::abs(_mass - (float)10.0) > 1e-10) {
      serializer->serialize_prop(mass_CSTR, _mass);
  }

}
