
#include "uuid.h"

#include <stdlib.h>
#include <ctime>


UUID::UUID(uuid_sequence_t *value, size_t hash)
    : _uuid(value), _hash(hash) {}

UUID::~UUID() { 
  MemoryPool<uuid_sequence_t>::delete_pod_object(_uuid);
}

UUID::UUID(UUID&& other)
{
  _uuid = other._uuid;
  _hash = other._hash;
  other._uuid = nullptr;
}

bool UUID::operator==(const UUID &other) const {
  if (_hash != other._hash)
    return false;
  if (this == &other)
    return true;
  return strcmp(*_uuid, *other._uuid) == 0;
}

bool UUID::operator<(const UUID &other) const { return _uuid < other._uuid; }

bool UUID::operator>(const UUID &other) const {
  return _uuid > other._uuid;
}


UUID &UUID::operator=(UUID &&other)
{
  if (_uuid)
    MemoryPool<uuid_sequence_t>::delete_pod_object(_uuid);
  _uuid = other._uuid;
  _hash = other._hash;
  other._uuid = nullptr;
  return *this;
}


UUID UUIDGenerator::generate() {
  static const char chrs[63] =
      "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

  uuid_sequence_t *uuid = MemoryPool<uuid_sequence_t>::new_pod_object();
  uuid_sequence_t& uuid_ref = *uuid;

  size_t hash = 0u;
  for (size_t i = 0; i < uuid_length; ++i) {
    char c = chrs[rand() % (sizeof(chrs) - 2)];
	uuid_ref[i] = c;
    hash = hash * 31 + c;
  }

  uuid_ref[uuid_length] = '\0'; // Only required so that c_str() works
  return UUID(uuid, hash);
}

void UUIDGenerator::init() {
  unsigned int seed = static_cast<unsigned int>(std::time(nullptr));
  std::srand(seed);
}
