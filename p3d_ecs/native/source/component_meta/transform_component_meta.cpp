
// AUTOGENERATED. DO NOT EDIT!
// Instead modify the component meta file.

#include "transform_component_meta.h"

#include "plain_text_serializer.h"

IMPLEMENT_COMPONENT_BASE(TransformComponent, TransformComponentMeta, 2u);

// AUTOGEN: member names for plain text serialization
const char* TransformComponentMeta::children_CSTR = "children";
const char* TransformComponentMeta::hpr_CSTR = "hpr";
const char* TransformComponentMeta::is_dirty_CSTR = "is_dirty";
const char* TransformComponentMeta::mat_CSTR = "mat";
const char* TransformComponentMeta::parent_CSTR = "parent";
const char* TransformComponentMeta::pos_CSTR = "pos";
const char* TransformComponentMeta::scale_CSTR = "scale";

void TransformComponentMeta::serialize(PlainTextSerializer* serializer) const {
  // Serialize children
  if (!_children.empty()) {
      serializer->serialize_prop_vec(children_CSTR, _children);
  }
  // Serialize hpr
  if (_hpr != LVecBase3f()) {
      serializer->serialize_prop(hpr_CSTR, _hpr);
  }
  // Serialize is_dirty
  if (_is_dirty != false) {
      serializer->serialize_prop(is_dirty_CSTR, _is_dirty);
  }
  // Serialize mat
  if (_mat != LMatrix4f(LMatrix4f::ident_mat())) {
      serializer->serialize_prop(mat_CSTR, _mat);
  }
  // Serialize parent
  if (_parent != nullptr) {
      serializer->serialize_prop(parent_CSTR, _parent);
  }
  // Serialize pos
  if (_pos != LVecBase3f()) {
      serializer->serialize_prop(pos_CSTR, _pos);
  }
  // Serialize scale
  if (_scale != LVecBase3f(1, 1, 1)) {
      serializer->serialize_prop(scale_CSTR, _scale);
  }

}
