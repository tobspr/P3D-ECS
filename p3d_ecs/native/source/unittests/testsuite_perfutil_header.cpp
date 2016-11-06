
#include "testsuite_perfutil_header.h"

#include "movement_system.h"
#include "unittest.h"
#include "perf_utility.h"

#include <iostream>
#include <unordered_set>
#include <functional>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

void
testsuite_perfutil_header() {

  using vec_t = vector<int>;
  using iter_t = vec_t::iterator;
  using const_iter_t = vec_t::const_iterator;

  auto make_intvec_5 = []() { return std::move(vec_t{ 53, 94, 38, 59, 3, 94 }); };

  BEGIN_TESTCASE("Perfutil: vector_find") {

    vec_t v_empty;
    TC_REQUIRE_TRUE(vector_find(v_empty, 123) == v_empty.end());

    auto v = make_intvec_5();
    for (auto elem : v) {
      TC_REQUIRE_TRUE(vector_find(v, elem) != v.end());
      TC_REQUIRE_EQ(*vector_find(v, elem), elem);
    }
    TC_REQUIRE_TRUE(vector_find(v, 123) == v.end());
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Perfutil: vector_find const") {

    const vec_t v_empty;
    TC_REQUIRE_TRUE(vector_find(v_empty, 123) == v_empty.end());

    const auto v = make_intvec_5();
    for (auto elem : v) {
      TC_REQUIRE_TRUE(vector_find(v, elem) != v.end());
      TC_REQUIRE_EQ(*vector_find(v, elem), elem);
    }
    TC_REQUIRE_TRUE(vector_find(v, 123) == v.end());
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Perfutil: vector_contains") {
    const vec_t v_empty;
    TC_REQUIRE_FALSE(vector_contains(v_empty, 123));

    const auto v = make_intvec_5();
    for (auto elem : v) {
      TC_REQUIRE_TRUE(vector_contains(v, elem));
    }
    TC_REQUIRE_FALSE(vector_contains(v, 123));
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Perfutil: float_equals") {
    TC_REQUIRE_FALSE(float_equals(1.0, 2.0));
    TC_REQUIRE_TRUE(float_equals(1.0, 1.0));
    TC_REQUIRE_TRUE(float_equals(1.0, 1.0));
    TC_REQUIRE_TRUE(float_equals(-1.0, -1.0));
    TC_REQUIRE_FALSE(float_equals(-1.0, 1.0));
    TC_REQUIRE_FALSE(float_equals(1.0, -1.0));

    const float eps = 1e-12;
    TC_REQUIRE_TRUE(float_equals(1.0, 1.0 + eps));
    TC_REQUIRE_TRUE(float_equals(1.0, 1.0 - eps));

    TC_REQUIRE_TRUE(float_equals(1.0 + eps, 1.0));
    TC_REQUIRE_TRUE(float_equals(1.0 - eps, 1.0));
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Perfutil: vector_erase_fast[iter]") {

    // Cases:
    // Case 1: We are removing the last element
    vec_t case_1 = { 1, 2, 3, 5 };
    vector_erase_fast(case_1, case_1.begin() + 3);
	vec_t case_1_exp = { 1, 2, 3 };
    TC_REQUIRE_EQ(case_1, case_1_exp);

    // Case 2: We are removing the one and only element
    vec_t case_2 = { 1 };
    vector_erase_fast(case_2, case_2.begin() + 0);
    TC_REQUIRE_EQ(case_2, {});

    // Case 3: We are removing the first element
    vec_t case_3 = { 1, 2, 3 };
    vector_erase_fast(case_3, case_3.begin() + 0);
	vec_t case_3_exp = { 3, 2 };
    TC_REQUIRE_EQ(case_3, case_3_exp); // order swapped during deletion
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Perfutil: vector_erase_fast[T]") {

    // Cases:
    // Case 1: We are removing the last element
    vec_t case_1 = { 1, 2, 3, 5 };
    vector_erase_fast(case_1, 5);
	vec_t case_1_exp = { 1, 2, 3 };
    TC_REQUIRE_EQ(case_1, case_1_exp);

    // Case 2: We are removing the one and only element
    vec_t case_2 = { 1 };
    vector_erase_fast(case_2, 1);
    TC_REQUIRE_EQ(case_2, {});

    // Case 3: We are removing the first element
    vec_t case_3 = { 1, 2, 3 };
    vector_erase_fast(case_3, 1);
	vec_t case_3_exp = { 3, 2 };
    TC_REQUIRE_EQ(case_3, case_3_exp); // order swapped during deletion
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Perfutil: vector_erase_fast_if_present[T]") {

    // Case 1: Empty
    vec_t case_1 = {};
    TC_REQUIRE_FALSE(vector_erase_fast_if_present(case_1, 123));
    TC_REQUIRE_EQ(case_1, {});

    // Case 2: Vector does not contain element (vector_find returns false)
    vec_t case_2 = { 1, 3, 5 };
    TC_REQUIRE_FALSE(vector_erase_fast_if_present(case_2, 2));
	vec_t case_2_exp = { 1, 3, 5 };
    TC_REQUIRE_EQ(case_2, case_2_exp);

    // Case 3: Vector contains element
    vec_t case_3 = { 1, 3, 5 };
    TC_REQUIRE_TRUE(vector_erase_fast_if_present(case_2, 3));
	vec_t case_3_exp = { 1, 5 };
    TC_REQUIRE_EQ(case_3, case_3_exp);
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Perfutil: compare_flat_sets") {

    // Case 1: Empty vectors
    vec_t case_1_v1 = {};
    vec_t case_1_v2 = {};
    TC_REQUIRE_TRUE(compare_flat_sets(case_1_v1, case_1_v2));

    // Case 2: Vectors with differing size
    vec_t case_2_v1 = { 1, 2, 3 };
    vec_t case_2_v2 = { 1, 2, 3, 4 };
    TC_REQUIRE_FALSE(compare_flat_sets(case_2_v1, case_2_v2));

    // Case 3: Vectors with 1 element (matching)
    vec_t case_3_v1 = { 1 };
    vec_t case_3_v2 = { 1 };
    TC_REQUIRE_TRUE(compare_flat_sets(case_3_v1, case_3_v2));

    // Case 4: Vectors with 1 element (not-matching)
    vec_t case_4_v1 = { 2 };
    vec_t case_4_v2 = { 3 };
    TC_REQUIRE_TRUE(compare_flat_sets(case_4_v1, case_4_v2));
    
    // Case 5: Vectors with multiple elements (matching, in order)
    vec_t case_5_v1 = { 1, 2, 3 };
    vec_t case_5_v2 = { 1, 2, 3 };
    TC_REQUIRE_TRUE(compare_flat_sets(case_5_v1, case_5_v2));
    
    // Case 6: Vectors with multiple elements (matching, not in order)
    vec_t case_6_v1 = { 1, 7, 2, 3 };
    vec_t case_6_v2 = { 1, 3, 2, 7 };
    TC_REQUIRE_TRUE(compare_flat_sets(case_6_v1, case_6_v2));
    
    // Case 7: Vectors with multiple elements (not matching)
    vec_t case_7_v1 = { 1, 7, 2, 3 };
    vec_t case_7_v2 = { 3, 8, 2, 7 };
    TC_REQUIRE_TRUE(compare_flat_sets(case_7_v1, case_7_v2));
  
  }
  END_TESTCASE;
};
