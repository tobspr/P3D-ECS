#pragma once

#ifndef P3D_ECS_UNITTEST_H
#define P3D_ECS_UNITTEST_H

#include "pandabase.h"
#include "entity_manager.h"
#include "output.h"

#include <iostream>
#include <string>
#include <functional>
#include <vector>
#include <initializer_list>

// Uncomment to avoid output overhead
// #define PROFILING

// Statistics used by the testrunner
struct UnittestStats {
  PUBLISHED :

    static size_t num_testcases_ran;
  static inline size_t get_num_testcases_ran() { return num_testcases_ran; };

  static size_t num_errors_occured;
  static inline size_t get_num_errors_occured() { return num_errors_occured; };

  static size_t num_requires_passed;
  static inline size_t get_num_requires_passed() { return num_requires_passed; };

  static size_t num_leaks;
  static inline size_t get_num_leaks() { return num_leaks; };

  static size_t num_uncatched_warnings;
  static inline size_t get_num_uncatched_warnings() { return num_uncatched_warnings; };
};

// Unittest helpers
namespace unittest {

  void measure_time(const ::std::string& desc, size_t num_iterations,
                    ::std::function<void()> method);

  template <typename T>
  inline ::std::string stringify(const T& value) {
    ::std::stringstream s;
    s << value;
    return s.str();
  }

  template <typename V>
  inline ::std::string stringify(const ::std::vector<V>& value) {
    ::std::stringstream s;
    s << "std::vector { size = " << value.size() << " }, contents = ";
    for (auto& v : value)
      s << v << " ";
    return s.str();
  }

  void testcase_fail(const ::std::string& value, const ::std::string& expected,
                     const ::std::string& comparison, const char* file, size_t line);

  template <typename T>
  bool expect(const T& value, const T& expected, const ::std::string& comparison, const char* file,
              size_t line) {
    if (value != expected) {
      testcase_fail(stringify(value), stringify(expected), comparison, file, line);
      ::UnittestStats::num_errors_occured++;
      return false;
    } else {
      ::UnittestStats::num_requires_passed++;
      return true;
    }
  }

  template <typename A, typename B>
  bool expect(A value, B expected, const ::std::string& comparison, const char* file, size_t line) {
    // Forced cast from B to A to avoid error when choosing two different types
    // (e.g. int and size_t)
    return expect(value, (A)expected, comparison, file, line);
  }

  void run_testcase(const ::std::string& name, const char* file, size_t line,
                    ::std::function<void(::EntityManager*)> inner);

  struct RequireWarningScoped {
    RequireWarningScoped(::std::initializer_list<::p3d_ecs::Warning> warnings);
    ~RequireWarningScoped();

  private:
    std::vector<::p3d_ecs::Warning> _warnings;
  };
}

#ifndef PROFILING
#define TC_STATUS(content) ::std::cout << "> " << content << ::std::endl;
#define REQUIRE_EQ(value, expected)                                                                \
  ::unittest::expect(value, expected, " " #value " == " #expected "", __FILE__, __LINE__)

#else
#define TC_STATUS(content)
#define REQUIRE_EQ(value, expected) ((value), true)
#define REQUIRE_EQ(value, expected) ((value), true)
#endif

// Convenience macros
#define REQUIRE_TRUE(value) REQUIRE_EQ(value, true)
#define REQUIRE_FALSE(value) REQUIRE_EQ(value, false)
#define REQUIRE_PTR(value) REQUIRE_TRUE(value != nullptr)

// Testcase
#define BEGIN_TESTCASE(name) ::unittest::run_testcase(name, __FILE__, __LINE__, [&](::EntityManager *mgr) -> void {
#define END_TESTCASE                                                                               \
  })

// Time Measure
#define BEGIN_MEASURE(name, iterations) ::unittest::measure_time(name, iterations, [&]() -> void {
#define END_MEASURE                                                                                \
  })

// Warnings
#define REQUIRE_WARNING_SCOPED(nr)                                                                 \
  ::unittest::RequireWarningScoped _expect_warning_##nr { ::p3d_ecs::Warning::##nr }
#define REQUIRE_WARNING_SCOPED2(nr1, nr2)                                                          \
  ::unittest::RequireWarningScoped _expect_warning_##nr1##_and_##nr2 {                             \
    ::p3d_ecs::Warning::##nr1, ::p3d_ecs::Warning::##nr2                                           \
  }
#define REQUIRE_WARNING_SCOPED3(nr1, nr2, nr3)                                                     \
  ::unittest::RequireWarningScoped _expect_warning_##nr1##_and_##nr2_and_##nr3 {                   \
    ::p3d_ecs::Warning::##nr1, ::p3d_ecs::Warning::##nr2, ::p3d_ecs::Warning::##nr3                \
  }

#endif