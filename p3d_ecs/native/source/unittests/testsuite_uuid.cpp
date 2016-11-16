
#include "testsuite_uuid.h"

#include "unittest.h"
#include "uuid.h"

void
testsuite_uuid() {

  BEGIN_TESTCASE("Generating simple UUID") {

    UUID uuid = UUID::generate();

    TC_STATUS("Generated uuid: " << uuid.c_str() << " with hash " << uuid.hash());
    REQUIRE_EQ(uuid.size(), UUID::uuid_length);
    REQUIRE_TRUE(UUID::is_valid_uuid(uuid.c_str()));
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Testing uuid move constructor") {

    UUID uuid = UUID::generate();
    UUID uuid2(std::move(uuid));

    TC_STATUS("Generated uuid: " << uuid2.c_str());
    REQUIRE_EQ(uuid2.size(), UUID::uuid_length);
    REQUIRE_TRUE(UUID::is_valid_uuid(uuid2.c_str()));
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Testing uuid operator =") {

    UUID uuid = UUID::generate();
    UUID uuid2 = UUID::generate();

    TC_STATUS("Generated uuid: " << uuid2.c_str());
    REQUIRE_TRUE(uuid.hash() != uuid2.hash());
    REQUIRE_TRUE(UUID::is_valid_uuid(uuid.c_str()));
    REQUIRE_TRUE(UUID::is_valid_uuid(uuid2.c_str()));
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Test UUID::operator== Identity") {
    for (size_t i = 0; i < 30; ++i) {
      UUID a = UUID::generate();
      REQUIRE_TRUE(a == a);
    }
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Test UUID::operator== Different Hash") {
    for (size_t i = 0; i < 30; ++i) {
      UUID a = UUID::generate();
      UUID b = UUID::generate();
      REQUIRE_FALSE(a == b);
    }
  }
  END_TESTCASE;

  auto make_empty_uuid = []() {
    UUID result = UUID::generate();
    UUID tmp = std::move(result);
    return std::move(result);
  };

  BEGIN_TESTCASE("Test UUID::operator== Nullptr [lhs]") {

    for (size_t i = 0; i < 30; ++i) {
      UUID lhs = make_empty_uuid();
      UUID rhs = UUID::generate();
      REQUIRE_FALSE(lhs == rhs);
    }
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Test UUID::operator== Nullptr [rhs]") {

    for (size_t i = 0; i < 30; ++i) {
      UUID lhs = UUID::generate();
      UUID rhs = make_empty_uuid();
      REQUIRE_FALSE(lhs == rhs);
    }
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Test UUID::operator== Nullptr [lhs+rhs]") {

    for (size_t i = 0; i < 30; ++i) {
      UUID lhs = make_empty_uuid();
      UUID rhs = make_empty_uuid();
      REQUIRE_TRUE(lhs == rhs);
    }
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Test UUID::operator== same string, different ptr") {

    UUID a = UUID::generate_faulty_for_testcases("AAAAAAAAAAAAAAAA");
    UUID a2 = UUID::generate_faulty_for_testcases("AAAAAAAAAAAAAAAA");
    UUID a3 = UUID::generate_faulty_for_testcases("AAAAAAAAAAAAAAAB");
    UUID a4 = UUID::generate_faulty_for_testcases("BAAAAAAAAAAAAAAA");
    UUID a5 = UUID::generate_faulty_for_testcases("AAAAAAAABAAAAAAA");

    REQUIRE_TRUE(a == a2);
    REQUIRE_TRUE(a2 == a);

    REQUIRE_FALSE(a == a3);
    REQUIRE_FALSE(a == a4);
    REQUIRE_FALSE(a == a5);
    REQUIRE_FALSE(a3 == a);
    REQUIRE_FALSE(a4 == a);
    REQUIRE_FALSE(a5 == a);
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Test UUID::operator< / operator>") {

    for (size_t i = 0; i < 30; ++i) {
      UUID a = UUID::generate();
      UUID b = UUID::generate();
      UUID c = UUID::generate();

      if (a < b)
        REQUIRE_TRUE(b > a);

      if (a > b)
        REQUIRE_TRUE(b < a);

      if (a < b && b < c)
        REQUIRE_TRUE(a < c);

      if (a > b && b > c)
        REQUIRE_TRUE(a > c);
    }
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Test UUID::operator=") {

    UUID a = UUID::generate();
    UUID b = std::move(a);

    UUID c = UUID::generate();
    UUID d = UUID::generate();
    UUID e = UUID::generate();

    d = std::move(e);
    c = std::move(d);
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Test UUID::is_valid_uuid") {

    // Case 1: empty string
    REQUIRE_FALSE(UUID::is_valid_uuid(UUID::generate_faulty_for_testcases("").c_str()));

    // Case 2: string with differing length
    REQUIRE_FALSE(UUID::is_valid_uuid(UUID::generate_faulty_for_testcases("absaef").c_str()));

    // Case 3: string with same length but invalid characters
    REQUIRE_FALSE(UUID::is_valid_uuid(UUID::generate_faulty_for_testcases("^BCABCABCABC").c_str()));

    // Case 4: string with valid characters, checking bounds
    REQUIRE_TRUE(
      UUID::is_valid_uuid(UUID::generate_faulty_for_testcases("azAZ09aaaaaaaaaa").c_str()));

    // Case 5: string with non-printable characters
    REQUIRE_FALSE(
      UUID::is_valid_uuid(UUID::generate_faulty_for_testcases("a\1aaaaaaaaaaaaaa").c_str()));

    // Case 6: valid uuid
    REQUIRE_TRUE(UUID::is_valid_uuid(UUID::generate().c_str()));
  }
  END_TESTCASE;
}