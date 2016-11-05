
#include "physics_component_meta.h"

#include "plain_text_serializer.h"

IMPLEMENT_COMPONENT_BASE(PhysicsComponent, PhysicsComponentMeta, 1u);

void PhysicsComponentMeta::serialize(PlainTextSerializer* serializer) const {
  // Serialize mass
  if (std::abs(_mass - (float)10.0) > 1e-10) {
      serializer->serialize_prop("mass", _mass);
  }
  // Serialize static
  if (_static != false) {
      serializer->serialize_prop("static", _static);
  }

}
