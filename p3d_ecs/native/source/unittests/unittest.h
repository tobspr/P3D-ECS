#pragma once

#ifndef P3D_ECS_UNITTEST_H
#define P3D_ECS_UNITTEST_H

#include "entity_manager.h"
#include "entity_system.h"
#include "all_components.h"
#include "entity.h"
#include "memory_pool.h"

#include <iostream>
#include <string>
#include <fstream>
#include <functional>

using namespace std; // Fine for the unittests

//#define PROFILING

#ifndef PROFILING

#define TC_STATUS(content) cout << "TC> " << content << endl;
#define TC_REQUIRE_EQ(value, expected)                                                                                 \
  testsuite_expect(value, expected, " " #value " == " #expected "", __FILE__, __LINE__);

#else
#define TC_STATUS(content) ;
#define TC_REQUIRE_EQ(value, expected) ;
#endif

#define TC_REQUIRE_TRUE(value) TC_REQUIRE_EQ(value, true)
#define TC_REQUIRE_FALSE(value) TC_REQUIRE_EQ(value, false)
#define TC_REQUIRE_PTR(value) TC_REQUIRE_TRUE(value != nullptr)

void write_tc_log(const string& msg);
void measure_time(const string& desc, size_t num_iterations, function<void()> method);

template <typename T>
void
testsuite_expect(T value, T expected, const string& comparison, const char* file, size_t line) {
  if (value != expected) {
    stringstream s;
    s << "\n\n================================================================="
         "======="
      << endl;
    s << "\n\nTESTCASE FAIL!" << endl;
    s << "In file " << file << " Line " << line << endl;
    s << "Expected: " << expected << endl;
    s << "Actual: " << value << endl;
    s << "Expression: " << comparison << endl;
    s << "\n\n================================================================="
         "======="
      << endl;
    s << "\n\n\n\n\n";
    write_tc_log(s.str());
    cerr << s.str();
  }
}

template <typename A, typename B>
void
testsuite_expect(A value, B expected, const string& comparison, const char* file, size_t line) {
  // Forced cast from B to A to avoid error when choosing two different types
  // (e.g. int and size_t)
  return testsuite_expect(value, (A)expected, comparison, file, line);
}

class EntityManager;
void run_testcase(const string& name, function<void(EntityManager*)> inner);

#define BEGIN_TESTCASE(name) run_testcase(name, [&](EntityManager *mgr) {
#define END_TESTCASE                                                                                                   \
  });

#define BEGIN_MEASURE(name, iterations) measure_time(name, iterations, [&]() {
#define END_MEASURE                                                                                                    \
  });

#endif