#pragma once

#ifndef P3D_ECS_UUID_H
#define P3D_ECS_UUID_H

#include <random>
#include <string>

#include "memory_pool.h"

class UUIDGenerator;

#define UUID_LENGTH 16

using uuid_sequence_t = char[UUID_LENGTH + 1];

// Class to represent uuids
class UUID {
  friend class UUIDGenerator;

  UUID(uuid_sequence_t *value, size_t hash);

public:
  UUID(UUID &&other);
  UUID &operator=(UUID &&other);
  ~UUID();

  UUID(const UUID &other) = delete;            // nope
  UUID &operator=(const UUID &other) = delete; // nope

  inline size_t size() const { return UUID_LENGTH; }
  inline size_t length() const { return UUID_LENGTH; }
  inline const char *c_str() const { return *_uuid; }
  inline size_t hash() const { return _hash; }

  bool operator==(const UUID &other) const;
  bool operator<(const UUID &other) const;
  bool operator>(const UUID &other) const;

private:
  uuid_sequence_t *_uuid;
  size_t _hash;
};

class UUIDGenerator {

public:
  // The length of all generated uuids
  static const size_t uuid_length = UUID_LENGTH;

  // Generates a new uuid
  // Duration: ~0.7 nanoseconds - use with care!
  static UUID generate();

  static void init();
};

#undef UUID_LENGTH

#endif