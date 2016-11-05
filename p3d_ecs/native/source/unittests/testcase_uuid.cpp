
#include "testcase_uuid.h"

#include "unittest.h"
#include "uuid.h"

#include <iostream>
#include <unordered_set>
#include <functional>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

void testcase_uuid() {
  general_testsuite("Generating simple UUID", [](EntityManager *mgr) {

    UUID uuid = UUIDGenerator::generate();

    TC_STATUS("Generated uuid: " << uuid.c_str() << " with hash "
                                 << uuid.hash());
    TC_EXPECT(uuid.size(), UUIDGenerator::uuid_length);
  });

  general_testsuite("Testing UUID performance", [](EntityManager *mgr) {

    size_t num_iterations = 30000000;
    measure_time("Generating and testing uuids",
                 [&]() {

                   for (size_t i = 0; i < num_iterations; ++i) {
                     UUID uuid = UUIDGenerator::generate();
                   }
                 },
                 num_iterations);

  });

  general_testsuite("Testing uuid move constructor", [](EntityManager *mgr) {

    UUID uuid = UUIDGenerator::generate();
    UUID uuid2(std::move(uuid));

    TC_STATUS("Generated uuid: " << uuid2.c_str());
    TC_EXPECT(uuid2.size(), UUIDGenerator::uuid_length);
  });

  general_testsuite("Testing uuid operator =", [](EntityManager *mgr) {

    UUID uuid = UUIDGenerator::generate();
    UUID uuid2 = UUIDGenerator::generate();

    TC_STATUS("Generated uuid: " << uuid2.c_str());
    TC_EXPECT(uuid.hash() != uuid2.hash(), true);
  });

  general_testsuite("Testing UUID for collisions", [](EntityManager *mgr) {

    size_t num_iterations = 5000000;
    measure_time(
        "Testing uuids",
        [&]() {
          std::unordered_set<UUID> uuids;
          uuids.reserve(num_iterations);
          bool result = false;
          for (size_t i = 0; i < num_iterations; ++i) {

            UUID uuid = UUIDGenerator::generate();

            auto result = uuids.emplace(std::move(uuid));

            TC_EXPECT(result.second, true);
            if (!result.second) {
              TC_STATUS("COLLISION!");
              TC_STATUS(
                  "After "
                  << i
                  << " iterations, the first collision occured. Stopping loop."
                  << endl
                  << endl
                  << endl);
              TC_STATUS("Collided element was: " << result.first->c_str());
              break; // Avoid spam
            }
          }
        },
        num_iterations);

  });
}
