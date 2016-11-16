#pragma once
#ifndef P3D_ECS_FAST_ENTITY_MAP_H
#define P3D_ECS_FAST_ENTITY_MAP_H

#include "config_module.h"
#include "perf_utility.h"
#include "uuid.h"
#include "entity.h"
#include "leak_detector.h"
#include <vector>

// Fast mapping from UUID to Entity
class FastEntityMap {
public:
  using bucket_t = std::vector<Entity*>;
  static const size_t default_reserve = 0;

  inline FastEntityMap(size_t bucket_count = 100000)
    : _buckets(new bucket_t[bucket_count])
    , _bucket_count(bucket_count) {
    ECS_ON_CREATE("FastEntityMap");
    if (default_reserve > 0) {
      for (size_t i = 0; i < bucket_count; ++i) {
        _buckets[i].reserve(default_reserve);
      }
    }
  };

  inline ~FastEntityMap() {
    ECS_ON_DELETE("FastEntityMap");
    delete[] _buckets;
  }

  inline Entity* find(const UUID& uuid) /* const */ {
    if (uuid.is_empty())
      return nullptr;
    for (Entity* entity : get_bucket(uuid)) {
      if (entity->get_uuid() == uuid)
        return entity;
    }
    return nullptr;
  }

  ECS_FORCEINLINE Entity* find(Entity* ptr) {
    ECS_ASSERT(ptr != nullptr); // TODO: Maybe just return nullptr instead of assertion?
    return find(ptr->get_uuid());
  }

  ECS_FORCEINLINE void insert(Entity* entity) {
    ECS_ASSERT(entity != nullptr);
    const UUID& uuid = entity->get_uuid();
    ECS_ASSERT(!uuid.is_empty());
    get_bucket(uuid).push_back(entity);
  }

  ECS_FORCEINLINE bool remove(Entity* entity) {
    const UUID& uuid = entity->get_uuid();
    ECS_ASSERT(!uuid.is_empty());
    return ::p3d_ecs::performance::vector_erase_if_contains(get_bucket(uuid), entity);
  }

  inline size_t get_memory_usage_bytes(size_t num_entities) const {
    size_t space_fixed = _bucket_count * (sizeof(bucket_t) + default_reserve * sizeof(Entity*));
    size_t space_dynamic = num_entities * sizeof(Entity*);
    return space_dynamic + space_fixed + sizeof(FastEntityMap);
  }

private:
  // TODO: Make a const/nonconst version of this and use it appropriately.
  ECS_FORCEINLINE bucket_t& get_bucket(const UUID& uuid) /* const */ {
    return _buckets[uuid.hash() % _bucket_count];
  }

  size_t _bucket_count;
  bucket_t* _buckets;
};

#endif