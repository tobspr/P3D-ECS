#pragma once

#ifndef P3D_ECS_UUID_H
#define P3D_ECS_UUID_H

#include <random>
#include <string>

#include "memory_pool.h"

class UUID;

#define UUID_LENGTH 16

using uuid_sequence_t = char[UUID_LENGTH + 1];

// Class to represent uuids
class UUID {

  UUID(uuid_sequence_t* value, size_t hash);

public:
  UUID();

  // Static methods
  static const size_t uuid_length = UUID_LENGTH;
  static UUID generate();
  static UUID generate_from_string(const std::string& str);
  static void init_uuid_rng();
  static UUID generate_faulty_for_testcases(const std::string& content);
  static bool is_valid_uuid(const std::string& text);

  UUID(UUID&& other);
  UUID& operator=(UUID&& other);
  ~UUID();

  UUID(const UUID& other) = delete;            // nope
  UUID& operator=(const UUID& other) = delete; // nope

  inline size_t size() const { return UUID_LENGTH; }
  inline size_t length() const { return UUID_LENGTH; }
  inline const char* c_str() const { return *_uuid; }
  inline size_t hash() const { return _hash; }
  inline bool is_empty() const { return _uuid == nullptr; }
  void clear();

  ECS_FORCEINLINE bool operator==(const UUID& other) const;
  bool operator<(const UUID& other) const;
  bool operator>(const UUID& other) const;

private:
  static size_t compute_hash(const char* uuid);

  uuid_sequence_t* _uuid;
  size_t _hash;
};

namespace std {
  template <>
  struct hash<UUID> {
    size_t operator()(const UUID& uuid) const { return uuid.hash(); }
  };
}

ECS_FORCEINLINE bool UUID::operator==(const UUID& other) const {
  if (_hash != other._hash)
    return false;
  if (this == &other)
    return true;
  if (*_uuid == nullptr || *other._uuid == nullptr)
    return *other._uuid == *_uuid;
  return strcmp(*_uuid, *other._uuid) == 0;
}

#undef UUID_LENGTH

#endif