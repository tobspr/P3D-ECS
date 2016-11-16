
#include "serialization_helpers.h"
#include "entity_manager.h"
#include "entity.h"
#include "output.h"

void
serialize_LMatrix4f(const LMatrix4f& src, ::p3d_ecs::proto::LMatrix4f* dest) {
  dest->set_m_0_0(src.get_cell(0, 0));
  dest->set_m_0_1(src.get_cell(0, 1));
  dest->set_m_0_2(src.get_cell(0, 2));
  dest->set_m_0_3(src.get_cell(0, 3));
  dest->set_m_1_0(src.get_cell(1, 0));
  dest->set_m_1_1(src.get_cell(1, 1));
  dest->set_m_1_2(src.get_cell(1, 2));
  dest->set_m_1_3(src.get_cell(1, 3));
  dest->set_m_2_0(src.get_cell(2, 0));
  dest->set_m_2_1(src.get_cell(2, 1));
  dest->set_m_2_2(src.get_cell(2, 2));
  dest->set_m_2_3(src.get_cell(2, 3));
  dest->set_m_3_0(src.get_cell(3, 0));
  dest->set_m_3_1(src.get_cell(3, 1));
  dest->set_m_3_2(src.get_cell(3, 2));
  dest->set_m_3_3(src.get_cell(3, 3));
}

void
deserialize_LMatrix4f(LMatrix4f& dest, const ::p3d_ecs::proto::LMatrix4f& src) {
  dest.set_cell(0, 0, src.m_0_0());
  dest.set_cell(0, 1, src.m_0_1());
  dest.set_cell(0, 2, src.m_0_2());
  dest.set_cell(0, 3, src.m_0_3());
  dest.set_cell(1, 0, src.m_1_0());
  dest.set_cell(1, 1, src.m_1_1());
  dest.set_cell(1, 2, src.m_1_2());
  dest.set_cell(1, 3, src.m_1_3());
  dest.set_cell(2, 0, src.m_2_0());
  dest.set_cell(2, 1, src.m_2_1());
  dest.set_cell(2, 2, src.m_2_2());
  dest.set_cell(2, 3, src.m_2_3());
  dest.set_cell(3, 0, src.m_3_0());
  dest.set_cell(3, 1, src.m_3_1());
  dest.set_cell(3, 2, src.m_3_2());
  dest.set_cell(3, 3, src.m_3_3());
}

void
deserialize_Entity(entity_ptr_t& dest, entity_ptr_cache_t& dest_fillin_cache,
                   const std::string& uuid) {
  if (uuid.size() == 0) {
    dest = nullptr;
  } else {
    if (!UUID::is_valid_uuid(uuid)) {
      ECS_EMIT_WARNING(SER_0005, "Invalid UUID for Entity*!");
      dest = nullptr;
    } else {
      dest_fillin_cache = UUID::generate_from_string(uuid);
    }
  }
}

void
deserialize_VectorOfEntity(entity_list_ptr_t& dest, entity_list_ptr_cache_t& dest_fillin_cache,
                           const child_list_t& src) {
  for (const std::string& elem : src) {
    if (!UUID::is_valid_uuid(elem)) {
      ECS_EMIT_WARNING(SER_0006, "Invalid UUID in vector of Entity*!");
      continue;
    }
    dest_fillin_cache.push_back(UUID::generate_from_string(elem));
  }
}

void
fillin_Entity(EntityManager* mgr, entity_ptr_cache_t& cache, entity_ptr_t& dest) {
  if (!cache.is_empty()) {
    Entity* ptr = mgr->find_entity(cache);
    cache.clear();
    if (ptr == nullptr) {
      ECS_EMIT_WARNING(SER_0007, "Tried to fillin ptr for property but UUID was not found!");
      dest = nullptr;
    } else {
      dest = ptr;
    }
  }
}

void
fillin_VectorOfEntity(EntityManager* mgr, entity_list_ptr_cache_t& cache, entity_list_ptr_t& dest) {
  for (auto& entry : cache) {
    ECS_ASSERT(!entry.is_empty()); // How could this even happen?
    Entity* entity = mgr->find_entity(entry);
    if (entity == nullptr) {
      ECS_EMIT_WARNING(SER_0008, "Entity* in vector has invalid UUID, skipping.");
    } else {
      dest.push_back(entity);
    }
  }
  cache.clear();
}