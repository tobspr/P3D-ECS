
#include "transform_component_meta.h"

#include "serialization.h"

IMPLEMENT_COMPONENT_BASE(TransformComponentMeta, 2u);

void TransformComponentMeta::serialize(PlainTextSerializer* serializer) const {
  // Serialize children
  if (!_children.empty()) {
      serializer->serialize_prop_vec("children", _children);
  }
  // Serialize hpr
  if (_hpr != LVecBase3f()) {
      serializer->serialize_prop("hpr", _hpr);
  }
  // Serialize is_dirty
  if (_is_dirty != false) {
      serializer->serialize_prop("is_dirty", _is_dirty);
  }
  // Serialize mat
  if (_mat != LMatrix4f(LMatrix4f::ident_mat())) {
      serializer->serialize_prop("mat", _mat);
  }
  // Serialize parent
  if (_parent != nullptr) {
      serializer->serialize_prop("parent", _parent);
  }
  // Serialize pos
  if (_pos != LVecBase3f()) {
      serializer->serialize_prop("pos", _pos);
  }
  // Serialize scale
  if (_scale != LVecBase3f(1, 1, 1)) {
      serializer->serialize_prop("scale", _scale);
  }

}
