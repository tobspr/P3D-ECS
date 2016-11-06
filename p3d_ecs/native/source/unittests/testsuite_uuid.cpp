
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

  BEGIN_TESTCASE("Test UUID::operator== Identity") {
    for (size_t i = 0; i < 30; ++i) {
      UUID a = UUIDGenerator::generate();
      TC_REQUIRE_TRUE(a == a);
    }
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Test UUID::operator== Different Hash") {
    for (size_t i = 0; i < 30; ++i) {
      UUID a = UUIDGenerator::generate();
      UUID b = UUIDGenerator::generate();
      TC_REQUIRE_FALSE(a == b);
    }
  }
  END_TESTCASE;

  auto make_empty_uuid = []() {
    UUID result = UUIDGenerator::generate();
    UUID tmp = std::move(result);
    return std::move(result);
  };

  BEGIN_TESTCASE("Test UUID::operator== Nullptr [lhs]") {

    for (size_t i = 0; i < 30; ++i) {
      UUID lhs = make_empty_uuid();
      UUID rhs = UUIDGenerator::generate();
      TC_REQUIRE_FALSE(lhs == rhs);
    }
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Test UUID::operator== Nullptr [rhs]") {

    for (size_t i = 0; i < 30; ++i) {
      UUID lhs = UUIDGenerator::generate();
      UUID rhs = make_empty_uuid();
      TC_REQUIRE_FALSE(lhs == rhs);
    }
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Test UUID::operator== Nullptr [lhs+rhs]") {

    for (size_t i = 0; i < 30; ++i) {
      UUID lhs = make_empty_uuid();
      UUID rhs = make_empty_uuid();
      TC_REQUIRE_TRUE(lhs == rhs);
    }
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Test UUID::operator== same string, different ptr") {

    UUID a = UUIDGenerator::generate_faulty_for_testcases("AAAAAAAAAAAAAAAA");
    UUID a2 = UUIDGenerator::generate_faulty_for_testcases("AAAAAAAAAAAAAAAA");
    UUID a3 = UUIDGenerator::generate_faulty_for_testcases("AAAAAAAAAAAAAAAB");
    UUID a4 = UUIDGenerator::generate_faulty_for_testcases("BAAAAAAAAAAAAAAA");
    UUID a5 = UUIDGenerator::generate_faulty_for_testcases("AAAAAAAABAAAAAAA");

    TC_REQUIRE_TRUE(a == a2);
    TC_REQUIRE_TRUE(a2 == a);

    TC_REQUIRE_FALSE(a == a3);
    TC_REQUIRE_FALSE(a == a4);
    TC_REQUIRE_FALSE(a == a5);
    TC_REQUIRE_FALSE(a3 == a);
    TC_REQUIRE_FALSE(a4 == a);
    TC_REQUIRE_FALSE(a5 == a);
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Test UUID::operator< / operator>") {

    for (size_t i = 0; i < 30; ++i) {
      UUID a = UUIDGenerator::generate();
      UUID b = UUIDGenerator::generate();
      UUID c = UUIDGenerator::generate();

      if (a < b)
        TC_REQUIRE_TRUE(b > a);

      if (a > b)
        TC_REQUIRE_TRUE(b < a);

      if (a < b && b < c)
        TC_REQUIRE_TRUE(a < c);

      if (a > b && b > c)
        TC_REQUIRE_TRUE(a > c);
    }
  }
  END_TESTCASE;
}