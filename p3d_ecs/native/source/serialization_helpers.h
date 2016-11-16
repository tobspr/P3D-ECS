#pragma once
#ifndef P3D_ECS_SERIALIZATION_HELPERS_H
#define P3D_ECS_SERIALIZATION_HELPERS_H

#ifndef INTERROGATE

#include "config_module.h"
#include "luse.h"
#include "luse.pb.h"

#include "uuid.h"
#include <string>
#include <vector>

class Entity;
class EntityManager;

void serialize_LMatrix4f(const LMatrix4f& src, ::p3d_ecs::proto::LMatrix4f* dest);
void deserialize_LMatrix4f(LMatrix4f& dest, const ::p3d_ecs::proto::LMatrix4f& src);

using entity_ptr_t = Entity*;
using entity_ptr_cache_t = UUID;
void deserialize_Entity(entity_ptr_t& dest, entity_ptr_cache_t& dest_fillin_cache,
                        const std::string& uuid);

using entity_list_ptr_t = std::vector<Entity*>;
using entity_list_ptr_cache_t = std::vector<UUID>;
using child_list_t = ::google::protobuf::RepeatedPtrField<::std::string>;
void deserialize_VectorOfEntity(entity_list_ptr_t& dest, entity_list_ptr_cache_t& dest_fillin_cache,
                                const child_list_t& src);

void fillin_Entity(EntityManager* mgr, entity_ptr_cache_t& cache, entity_ptr_t& dest);

void fillin_VectorOfEntity(EntityManager* mgr, entity_list_ptr_cache_t& cache,
                           entity_list_ptr_t& dest);

#endif
#endif