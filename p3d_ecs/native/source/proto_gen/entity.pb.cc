// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: entity.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "entity.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/port.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace p3d_ecs {
namespace proto {

namespace {

const ::google::protobuf::Descriptor* Entity_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  Entity_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_entity_2eproto() GOOGLE_ATTRIBUTE_COLD;
void protobuf_AssignDesc_entity_2eproto() {
  protobuf_AddDesc_entity_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "entity.proto");
  GOOGLE_CHECK(file != NULL);
  Entity_descriptor_ = file->message_type(0);
  static const int Entity_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Entity, uuid_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Entity, components_),
  };
  Entity_reflection_ =
    ::google::protobuf::internal::GeneratedMessageReflection::NewGeneratedMessageReflection(
      Entity_descriptor_,
      Entity::internal_default_instance(),
      Entity_offsets_,
      -1,
      -1,
      -1,
      sizeof(Entity),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Entity, _internal_metadata_));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_entity_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) GOOGLE_ATTRIBUTE_COLD;
void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
      Entity_descriptor_, Entity::internal_default_instance());
}

}  // namespace

void protobuf_ShutdownFile_entity_2eproto() {
  Entity_default_instance_.Shutdown();
  delete Entity_reflection_;
}

void protobuf_InitDefaults_entity_2eproto_impl() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::p3d_ecs::proto::protobuf_InitDefaults_components_2eproto();
  ::google::protobuf::internal::GetEmptyString();
  Entity_default_instance_.DefaultConstruct();
  Entity_default_instance_.get_mutable()->InitAsDefaultInstance();
}

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_InitDefaults_entity_2eproto_once_);
void protobuf_InitDefaults_entity_2eproto() {
  ::google::protobuf::GoogleOnceInit(&protobuf_InitDefaults_entity_2eproto_once_,
                 &protobuf_InitDefaults_entity_2eproto_impl);
}
void protobuf_AddDesc_entity_2eproto_impl() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  protobuf_InitDefaults_entity_2eproto();
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\014entity.proto\022\rp3d_ecs.proto\032\020component"
    "s.proto\"E\n\006Entity\022\014\n\004uuid\030\001 \001(\014\022-\n\ncompo"
    "nents\030\002 \001(\0132\031.p3d_ecs.proto.Componentsb\006"
    "proto3", 126);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "entity.proto", &protobuf_RegisterTypes);
  ::p3d_ecs::proto::protobuf_AddDesc_components_2eproto();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_entity_2eproto);
}

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AddDesc_entity_2eproto_once_);
void protobuf_AddDesc_entity_2eproto() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AddDesc_entity_2eproto_once_,
                 &protobuf_AddDesc_entity_2eproto_impl);
}
// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_entity_2eproto {
  StaticDescriptorInitializer_entity_2eproto() {
    protobuf_AddDesc_entity_2eproto();
  }
} static_descriptor_initializer_entity_2eproto_;

namespace {

static void MergeFromFail(int line) GOOGLE_ATTRIBUTE_COLD GOOGLE_ATTRIBUTE_NORETURN;
static void MergeFromFail(int line) {
  ::google::protobuf::internal::MergeFromFail(__FILE__, line);
}

}  // namespace


// ===================================================================

#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int Entity::kUuidFieldNumber;
const int Entity::kComponentsFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

Entity::Entity()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  if (this != internal_default_instance()) protobuf_InitDefaults_entity_2eproto();
  SharedCtor();
  // @@protoc_insertion_point(constructor:p3d_ecs.proto.Entity)
}

void Entity::InitAsDefaultInstance() {
  components_ = const_cast< ::p3d_ecs::proto::Components*>(
      ::p3d_ecs::proto::Components::internal_default_instance());
}

Entity::Entity(const Entity& from)
  : ::google::protobuf::Message(),
    _internal_metadata_(NULL) {
  SharedCtor();
  UnsafeMergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:p3d_ecs.proto.Entity)
}

void Entity::SharedCtor() {
  uuid_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  components_ = NULL;
  _cached_size_ = 0;
}

Entity::~Entity() {
  // @@protoc_insertion_point(destructor:p3d_ecs.proto.Entity)
  SharedDtor();
}

void Entity::SharedDtor() {
  uuid_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (this != &Entity_default_instance_.get()) {
    delete components_;
  }
}

void Entity::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* Entity::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return Entity_descriptor_;
}

const Entity& Entity::default_instance() {
  protobuf_InitDefaults_entity_2eproto();
  return *internal_default_instance();
}

::google::protobuf::internal::ExplicitlyConstructed<Entity> Entity_default_instance_;

Entity* Entity::New(::google::protobuf::Arena* arena) const {
  Entity* n = new Entity;
  if (arena != NULL) {
    arena->Own(n);
  }
  return n;
}

void Entity::Clear() {
// @@protoc_insertion_point(message_clear_start:p3d_ecs.proto.Entity)
  uuid_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (GetArenaNoVirtual() == NULL && components_ != NULL) delete components_;
  components_ = NULL;
}

bool Entity::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!GOOGLE_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:p3d_ecs.proto.Entity)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional bytes uuid = 1;
      case 1: {
        if (tag == 10) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->mutable_uuid()));
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(18)) goto parse_components;
        break;
      }

      // optional .p3d_ecs.proto.Components components = 2;
      case 2: {
        if (tag == 18) {
         parse_components:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
               input, mutable_components()));
        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormatLite::SkipField(input, tag));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:p3d_ecs.proto.Entity)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:p3d_ecs.proto.Entity)
  return false;
#undef DO_
}

void Entity::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:p3d_ecs.proto.Entity)
  // optional bytes uuid = 1;
  if (this->uuid().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::WriteBytesMaybeAliased(
      1, this->uuid(), output);
  }

  // optional .p3d_ecs.proto.Components components = 2;
  if (this->has_components()) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      2, *this->components_, output);
  }

  // @@protoc_insertion_point(serialize_end:p3d_ecs.proto.Entity)
}

::google::protobuf::uint8* Entity::InternalSerializeWithCachedSizesToArray(
    bool deterministic, ::google::protobuf::uint8* target) const {
  (void)deterministic; // Unused
  // @@protoc_insertion_point(serialize_to_array_start:p3d_ecs.proto.Entity)
  // optional bytes uuid = 1;
  if (this->uuid().size() > 0) {
    target =
      ::google::protobuf::internal::WireFormatLite::WriteBytesToArray(
        1, this->uuid(), target);
  }

  // optional .p3d_ecs.proto.Components components = 2;
  if (this->has_components()) {
    target = ::google::protobuf::internal::WireFormatLite::
      InternalWriteMessageNoVirtualToArray(
        2, *this->components_, false, target);
  }

  // @@protoc_insertion_point(serialize_to_array_end:p3d_ecs.proto.Entity)
  return target;
}

size_t Entity::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:p3d_ecs.proto.Entity)
  size_t total_size = 0;

  // optional bytes uuid = 1;
  if (this->uuid().size() > 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::BytesSize(
        this->uuid());
  }

  // optional .p3d_ecs.proto.Components components = 2;
  if (this->has_components()) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        *this->components_);
  }

  int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = cached_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void Entity::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:p3d_ecs.proto.Entity)
  if (GOOGLE_PREDICT_FALSE(&from == this)) MergeFromFail(__LINE__);
  const Entity* source =
      ::google::protobuf::internal::DynamicCastToGenerated<const Entity>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:p3d_ecs.proto.Entity)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:p3d_ecs.proto.Entity)
    UnsafeMergeFrom(*source);
  }
}

void Entity::MergeFrom(const Entity& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:p3d_ecs.proto.Entity)
  if (GOOGLE_PREDICT_TRUE(&from != this)) {
    UnsafeMergeFrom(from);
  } else {
    MergeFromFail(__LINE__);
  }
}

void Entity::UnsafeMergeFrom(const Entity& from) {
  GOOGLE_DCHECK(&from != this);
  if (from.uuid().size() > 0) {

    uuid_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.uuid_);
  }
  if (from.has_components()) {
    mutable_components()->::p3d_ecs::proto::Components::MergeFrom(from.components());
  }
}

void Entity::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:p3d_ecs.proto.Entity)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Entity::CopyFrom(const Entity& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:p3d_ecs.proto.Entity)
  if (&from == this) return;
  Clear();
  UnsafeMergeFrom(from);
}

bool Entity::IsInitialized() const {

  return true;
}

void Entity::Swap(Entity* other) {
  if (other == this) return;
  InternalSwap(other);
}
void Entity::InternalSwap(Entity* other) {
  uuid_.Swap(&other->uuid_);
  std::swap(components_, other->components_);
  _internal_metadata_.Swap(&other->_internal_metadata_);
  std::swap(_cached_size_, other->_cached_size_);
}

::google::protobuf::Metadata Entity::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = Entity_descriptor_;
  metadata.reflection = Entity_reflection_;
  return metadata;
}

#if PROTOBUF_INLINE_NOT_IN_HEADERS
// Entity

// optional bytes uuid = 1;
void Entity::clear_uuid() {
  uuid_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
const ::std::string& Entity::uuid() const {
  // @@protoc_insertion_point(field_get:p3d_ecs.proto.Entity.uuid)
  return uuid_.GetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
void Entity::set_uuid(const ::std::string& value) {
  
  uuid_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:p3d_ecs.proto.Entity.uuid)
}
void Entity::set_uuid(const char* value) {
  
  uuid_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:p3d_ecs.proto.Entity.uuid)
}
void Entity::set_uuid(const void* value, size_t size) {
  
  uuid_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:p3d_ecs.proto.Entity.uuid)
}
::std::string* Entity::mutable_uuid() {
  
  // @@protoc_insertion_point(field_mutable:p3d_ecs.proto.Entity.uuid)
  return uuid_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
::std::string* Entity::release_uuid() {
  // @@protoc_insertion_point(field_release:p3d_ecs.proto.Entity.uuid)
  
  return uuid_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
void Entity::set_allocated_uuid(::std::string* uuid) {
  if (uuid != NULL) {
    
  } else {
    
  }
  uuid_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), uuid);
  // @@protoc_insertion_point(field_set_allocated:p3d_ecs.proto.Entity.uuid)
}

// optional .p3d_ecs.proto.Components components = 2;
bool Entity::has_components() const {
  return this != internal_default_instance() && components_ != NULL;
}
void Entity::clear_components() {
  if (GetArenaNoVirtual() == NULL && components_ != NULL) delete components_;
  components_ = NULL;
}
const ::p3d_ecs::proto::Components& Entity::components() const {
  // @@protoc_insertion_point(field_get:p3d_ecs.proto.Entity.components)
  return components_ != NULL ? *components_
                         : *::p3d_ecs::proto::Components::internal_default_instance();
}
::p3d_ecs::proto::Components* Entity::mutable_components() {
  
  if (components_ == NULL) {
    components_ = new ::p3d_ecs::proto::Components;
  }
  // @@protoc_insertion_point(field_mutable:p3d_ecs.proto.Entity.components)
  return components_;
}
::p3d_ecs::proto::Components* Entity::release_components() {
  // @@protoc_insertion_point(field_release:p3d_ecs.proto.Entity.components)
  
  ::p3d_ecs::proto::Components* temp = components_;
  components_ = NULL;
  return temp;
}
void Entity::set_allocated_components(::p3d_ecs::proto::Components* components) {
  delete components_;
  components_ = components;
  if (components) {
    
  } else {
    
  }
  // @@protoc_insertion_point(field_set_allocated:p3d_ecs.proto.Entity.components)
}

inline const Entity* Entity::internal_default_instance() {
  return &Entity_default_instance_.get();
}
#endif  // PROTOBUF_INLINE_NOT_IN_HEADERS

// @@protoc_insertion_point(namespace_scope)

}  // namespace proto
}  // namespace p3d_ecs

// @@protoc_insertion_point(global_scope)