
#include "physics_component_meta.h"

#include "serialization.h"

IMPLEMENT_COMPONENT_BASE(PhysicsComponentMeta, 1u);

void PhysicsComponentMeta::serialize(YAMLSerializer* serializer) const {
  // Serialize hpr
  if (_hpr != LVecBase2f()) {
      serializer->serialize_prop("hpr", _hpr);
  }
  // Serialize mass
  if (std::abs(_mass - (float)10.0) > 1e-10) {
      serializer->serialize_prop("mass", _mass);
  }

}
