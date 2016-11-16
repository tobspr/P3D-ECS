
#include "uuid.h"
#include "config_module.h"
#include "leak_detector.h"

#include <stdlib.h>
#include <chrono>
#include <cassert>
#include <cstring>
#include <ctime>
#include <algorithm>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

using namespace std::chrono;

static unsigned int rand_seed;

inline int
rand_fast_16bit() {
  rand_seed = (214013 * rand_seed + 2531011);
  return (rand_seed >> 16) & 0xFFFF;
}

UUID::UUID()
  : _uuid(nullptr)
  , _hash(0u) {
  ECS_ON_CREATE("UUID");
}

UUID::UUID(uuid_sequence_t* value, size_t hash)
  : _uuid(value)
  , _hash(hash) {
  ECS_ON_CREATE("UUID");
}

UUID::~UUID() {
  MemoryPool<uuid_sequence_t>::delete_pod_object(_uuid);
  ECS_ON_DELETE("UUID");
}

UUID::UUID(UUID&& other) {
  // No need to free previous memory, since we are just about to get constructed
  ECS_ON_CREATE("UUID");
  _uuid = other._uuid;
  _hash = other._hash;
  other._uuid = nullptr;
  other._hash = 0u;
}

bool UUID::operator<(const UUID& other) const {
  return _hash < other._hash;
}

bool UUID::operator>(const UUID& other) const {
  return _hash > other._hash;
}

UUID& UUID::operator=(UUID&& other) {
  clear();
  _uuid = other._uuid;
  _hash = other._hash;
  other._uuid = nullptr;
  other._hash = 0u;
  return *this;
}

bool
UUID::is_valid_uuid(const std::string& text) {
  if (text.size() != uuid_length)
    return false;
  for (size_t i = 0; i < uuid_length; ++i) {
    char c = text[i];
    if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
      // Fine
    } else {
      return false;
    }
  }
  return true;
}

void
UUID::clear() {
  if (_uuid)
    MemoryPool<uuid_sequence_t>::delete_pod_object(_uuid);
  _uuid = nullptr;
  _hash = 0u;
}

size_t
UUID::compute_hash(const char* uuid) {
  size_t hash = 0;
  for (size_t i = 0; i < uuid_length; i += 4) {
    // Unrolled for 4 loops, each iteration is: hash = hash * 31 + uuid[i]
    hash = hash * 923521 + uuid[i] * 29791 + uuid[i + 1] * 961 + uuid[i + 2] * 31 + uuid[i + 3];
  };
  return hash;
}

UUID
UUID::generate() {
  static const char chrs[66] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJK"
                               "LMNOPQRSTUVWXYZabc"; // pad to 64

  uuid_sequence_t* uuid = MemoryPool<uuid_sequence_t>::new_pod_object();
  uuid_sequence_t& uuid_ref = *uuid;

#ifdef WIN32
  // QueryPerformanceCounter is much faster on windows, so use that.
  LARGE_INTEGER ms;
  QueryPerformanceCounter(&ms);
  uint64_t count = static_cast<uint64_t>(ms.QuadPart);

#else
  nanoseconds ms = duration_cast<nanoseconds>(system_clock::now().time_since_epoch());
  // Last 6 bits are usually garbarge
  uint64_t count = ms.count() >> 6;
#endif

  // First 4 chars encode the nanoseconds from the 01.01.1970.
  // Each char stores 6 bit (=64 combinations) so we consume 16 bit in total for
  // this
  for (size_t i = 0; i < 4; ++i) {
    uuid_ref[i] = chrs[(count >> (6 * i)) & 0x3F];
  }

  // Fill rest of number with randoms, because its very likely that we will
  // create
  // two UUID's in the same millisecond.
  for (size_t i = 4; i < uuid_length - 1; i += 2) {
    int c = rand_fast_16bit();
    char c1 = chrs[c & 0x3F], c2 = chrs[(c >> 6) & 0x3F];
    uuid_ref[i] = c1;
    uuid_ref[i + 1] = c2;
  }

  // uuid_ref[uuid_length] = '\0'; // Only required so that c_str() works and we can use strcmp
  size_t hash = UUID::compute_hash(uuid_ref);
  return UUID(uuid, hash);
}

void
UUID::init_uuid_rng() {
  rand_seed = static_cast<unsigned int>(std::time(nullptr));
  std::srand(rand_seed);
}

UUID
UUID::generate_from_string(const std::string& str) {
  ECS_ASSERT(str.size() == uuid_length);
  uuid_sequence_t* uuid = MemoryPool<uuid_sequence_t>::new_pod_object();
  uuid_sequence_t& uuid_ref = *uuid;
  memcpy(uuid_ref, str.c_str(), uuid_length + 1);
  size_t hash = UUID::compute_hash(uuid_ref);
  return UUID(uuid, hash);
}

UUID
UUID::generate_faulty_for_testcases(const std::string& content) {
  uuid_sequence_t* uuid = MemoryPool<uuid_sequence_t>::new_pod_object();
  uuid_sequence_t& uuid_ref = *uuid;
  memset(uuid_ref, 0x00, uuid_length + 1);
  memcpy(uuid_ref, content.c_str(), std::min<size_t>(content.size(), uuid_length));
  size_t hash = UUID::compute_hash(uuid_ref);
  return UUID(uuid, hash);
}
