
#include "uuid.h"

#include <stdlib.h>
#include <chrono>
#include <cassert>
#include <cstring>
#include <ctime>

using namespace std::chrono;

UUID::UUID(uuid_sequence_t* value, size_t hash)
  : _uuid(value)
  , _hash(hash) {}

UUID::~UUID() {
  MemoryPool<uuid_sequence_t>::delete_pod_object(_uuid);
}

UUID::UUID(UUID&& other) {
  _uuid = other._uuid;
  _hash = other._hash;
  other._uuid = nullptr;
  other._hash = 0u;
}

bool UUID::operator==(const UUID& other) const {
  if (_hash != other._hash)
    return false;
  if (this == &other)
    return true;
  if (*_uuid == nullptr || *other._uuid == nullptr)
    return *other._uuid == *_uuid;
  return strcmp(*_uuid, *other._uuid) == 0;
}

bool UUID::operator<(const UUID& other) const {
  return _hash < other._hash;
}

bool UUID::operator>(const UUID& other) const {
  return _hash > other._hash;
}

UUID& UUID::operator=(UUID&& other) {
  if (_uuid)
    MemoryPool<uuid_sequence_t>::delete_pod_object(_uuid);
  _uuid = other._uuid;
  _hash = other._hash;
  other._uuid = nullptr;
  other._hash = 0u;
  return *this;
}

static unsigned int rand_seed;

inline int
rand_fast_16bit() {
  rand_seed = (214013 * rand_seed + 2531011);
  return (rand_seed >> 16) & 0xFFFF;
}

UUID
UUIDGenerator::generate() {
  static const char chrs[66] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJK"
                               "LMNOPQRSTUVWXYZabc"; // pad to 64

  uuid_sequence_t* uuid = MemoryPool<uuid_sequence_t>::new_pod_object();
  uuid_sequence_t& uuid_ref = *uuid;

  nanoseconds ms = duration_cast<nanoseconds>(system_clock::now().time_since_epoch());

  // Last 6 bits are usually garbarge
  uint64_t count = (ms.count() >> 6) & 0xFFFFFF;
  size_t hash = count;

  // First 4 chars encode the nanoseconds from the 01.01.1970.
  // Each char stores 6 bit (=64 combinations) so we consume 16 bit in total for
  // this
  for (size_t i = 0; i < 4; ++i) {
    uuid_ref[i] = chrs[count & 0x3F];
    count >>= 6;
  }

  // Fill rest of number with randoms, because its very likely that we will
  // create
  // two UUID's in the same millisecond.
  for (size_t i = 4; i < uuid_length - 1; i += 2) {
    int c = rand_fast_16bit();
    char c1 = chrs[c & 0x3F], c2 = chrs[(c >> 6) & 0x3F];
    uuid_ref[i] = c1;
    uuid_ref[i + 1] = c2;
    hash = hash * 961 + c1 * 31 + c2;
  }

  uuid_ref[uuid_length] = '\0'; // Only required so that c_str() works and we can use strcmp
  return UUID(uuid, hash);
}

void
UUIDGenerator::init() {
  rand_seed = static_cast<unsigned int>(std::time(nullptr));
  // std::srand(seed);
}


UUID UUIDGenerator::generate_faulty_for_testcases(const std::string& content) {
  uuid_sequence_t* uuid = MemoryPool<uuid_sequence_t>::new_pod_object();
  uuid_sequence_t& uuid_ref = *uuid;
  assert(content.size() == uuid_length);

  // todo: could use std::copy as well
  for(size_t i = 0; i < uuid_length; ++i) {
    uuid_ref[i] = content[i];
  }
  uuid_ref[uuid_length] = '\0';
  size_t hash = std::hash<std::string>()(content);
  return UUID(uuid, hash);
}
