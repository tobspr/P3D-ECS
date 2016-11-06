
#include "testsuite_uuid.h"

#include "unittest.h"
#include "uuid.h"

#include <iostream>
#include <unordered_set>
#include <functional>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

void
testsuite_uuid() {

  BEGIN_TESTCASE("Generating simple UUID") {

    UUID uuid = UUIDGenerator::generate();

    TC_STATUS("Generated uuid: " << uuid.c_str() << " with hash " << uuid.hash());
    TC_REQUIRE_EQ(uuid.size(), UUIDGenerator::uuid_length);
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Testing UUID performance") {

    size_t num_iterations = 30000000;
    BEGIN_MEASURE("Generating and testing uuids", num_iterations) {
      for (size_t i = 0; i < num_iterations; ++i) {
        UUID uuid = UUIDGenerator::generate();
      }
    }
    END_MEASURE;
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Testing uuid move constructor") {

    UUID uuid = UUIDGenerator::generate();
    UUID uuid2(std::move(uuid));

    TC_STATUS("Generated uuid: " << uuid2.c_str());
    TC_REQUIRE_EQ(uuid2.size(), UUIDGenerator::uuid_length);
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Testing uuid operator =") {

    UUID uuid = UUIDGenerator::generate();
    UUID uuid2 = UUIDGenerator::generate();

    TC_STATUS("Generated uuid: " << uuid2.c_str());
    TC_REQUIRE_TRUE(uuid.hash() != uuid2.hash());
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Testing UUID for collisions") {

    // size_t num_iterations = 5000000;
    size_t num_iterations = 50000;

    BEGIN_MEASURE("Testing uuids", num_iterations) {
      std::unordered_set<UUID> uuids;
      uuids.reserve(num_iterations);
      bool result = false;
      for (size_t i = 0; i < num_iterations; ++i) {

        UUID uuid = UUIDGenerator::generate();

        auto result = uuids.emplace(std::move(uuid));
        TC_REQUIRE_TRUE(result.second);

        if (!result.second) {
          TC_STATUS("COLLISION!");
          TC_STATUS("After " << i << " iterations, the first collision occured. Stopping loop." << endl
                             << endl
                             << endl);
          TC_STATUS("Collided element was: " << result.first->c_str());
          break; // Avoid spam
        }
      }
    }
    END_MEASURE;
  }
  END_TESTCASE;
}
