

#include "unittest.h"

#include "memory_pool.h"
#include "entity_manager.h"
#include "entity_system.h"
#include "entity.h"
#include "output.h"
#include "leak_detector.h"
#include "transform_component.h"
#include "physics_component.h"
#include "perf_utility.h"
#include "termcolor.h"

#include <fstream>
#include <chrono>
#include <iomanip>
#include <locale>
#include <algorithm>

size_t UnittestStats::num_testcases_ran = 0;
size_t UnittestStats::num_errors_occured = 0;
size_t UnittestStats::num_requires_passed = 0;
size_t UnittestStats::num_leaks = 0;
size_t UnittestStats::num_uncatched_warnings = 0;

namespace unittest {

  static bool unit_error_state = false;

  void log_testcase_error(const ::std::string& msg) {
#ifndef PROFILING
    ::std::ofstream outfile("test-errors.txt", ::std::ios_base::app);
    outfile << msg;
#endif
  };

  void measure_time(const ::std::string& desc, size_t num_iterations,
                    ::std::function<void()> method) {

    cout.imbue(std::locale(""));

    std::string pad(std::max<int>(0, 22 - (int)desc.size()), ' ');
    std::cout << "  " << termcolor::magenta << desc << pad << " " << setw(11) << num_iterations
              << " " << termcolor::reset;
    auto begin = chrono::high_resolution_clock::now();
    method();
    auto end = chrono::high_resolution_clock::now();

    size_t ms = chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    ms = std::max<size_t>(ms, 1); // Prevent bad output
    std::cout << termcolor::green << "  " << setw(5) << ms << " ms" << termcolor::reset;

    if (num_iterations > 1) {
      float ns_per_iteration = (float)ms / (float)num_iterations * 1000.0;
      std::cout << "  [" << setw(7) << setprecision(4) << fixed << ns_per_iteration << " ns/iter]"
                << termcolor::reset;

      int iteration_per_s = (int)((float)num_iterations / ms * 1000.0);
      std::cout << "  [" << setw(12) << fixed << iteration_per_s << " iter/s]" << termcolor::reset;
    }

    std::cout << std::endl;
  }

  void testcase_fail(const ::std::string& value, const ::std::string& expected,
                     const ::std::string& comparison, const char* file, size_t line) {
    unit_error_state = true;
    ::std::stringstream s;
    s << "\n\n================================================================\n";
    s << "TESTCASE FAIL!" << endl;
    s << "In file " << file << " Line " << line << endl;
    s << "Expected:   " << stringify(expected) << endl;
    s << "Actual:     " << stringify(value) << endl;
    s << "Expression: " << comparison << endl;
    s << "\n================================================================\n";
    s << "\n\n";
    log_testcase_error(s.str());
    cerr << termcolor::red << s.str() << termcolor::reset;
  }

  void run_testcase(const ::std::string& name, const char* file, size_t line,
                    function<void(::EntityManager*)> inner) {

    // #ifndef PROFILING
    std::cout << termcolor::green << endl
              << "[  TEST  ]" << termcolor::reset << "  " << name << std::endl;
    // std::cout << termcolor::grey << file << ": " << line << termcolor::reset << std::endl;
    // #endif

    unit_error_state = false;

    // Require a clean startup
    REQUIRE_EQ(::p3d_ecs::MessageManager::get_occurred_warnings(), {});

    // Make sure we don't reach the maximum id
    ::Entity::reset_id_pool();
    ::UnittestStats::num_testcases_ran++;

    // Reset memory pools
    ::MemoryPool<Entity>::reset();
    ::MemoryPool<UUID>::reset();
    ::MemoryPool<TransformComponent>::reset();
    ::MemoryPool<PhysicsComponent>::reset();

    // Reset memory leak counter
    ::p3d_ecs::leak_detector::reset_leaks();

    // Run actual testcase
    {
      ::EntityManager* mgr = new ::EntityManager();
      inner(mgr);
      mgr->reset();

      REQUIRE_EQ(mgr->get_num_entities(), 0);
      delete mgr;
    }

    // Check for leaks
    size_t new_leaks = ::p3d_ecs::leak_detector::get_num_leaks();
    ::UnittestStats::num_leaks += new_leaks;
    if (new_leaks != 0)
      unit_error_state = true;

    if (::MemoryPool<::Entity>::get_object_count() != 0) {
      ECS_EMIT_WARNING(SYS_0001, "MemoryPool<Entity> LEAK! "
                                   << ::MemoryPool<::Entity>::get_object_count()
                                   << " Objects not freed.");
      ::UnittestStats::num_leaks += MemoryPool<Entity>::get_object_count();
      unit_error_state = true;
    }
    if (::MemoryPool<::UUID>::get_object_count() != 0) {
      ECS_EMIT_WARNING(SYS_0001, "MemoryPool<UUID> LEAK! "
                                   << ::MemoryPool<::UUID>::get_object_count()
                                   << " Objects not freed.");
      ::UnittestStats::num_leaks += ::MemoryPool<::UUID>::get_object_count();
      unit_error_state = true;
    }
    if (::MemoryPool<::TransformComponent>::get_object_count() != 0) {
      ECS_EMIT_WARNING(SYS_0001, "MemoryPool<TransformComponent> LEAK! "
                                   << ::MemoryPool<::TransformComponent>::get_object_count()
                                   << " Objects not freed.");
      ::UnittestStats::num_leaks += MemoryPool<::TransformComponent>::get_object_count();
      unit_error_state = true;
    }
    if (::MemoryPool<::PhysicsComponent>::get_object_count() != 0) {
      ECS_EMIT_WARNING(SYS_0001, "MemoryPool<PhysicsComponent> LEAK! "
                                   << ::MemoryPool<::PhysicsComponent>::get_object_count()
                                   << " Objects not freed.");
      ::UnittestStats::num_leaks += MemoryPool<::PhysicsComponent>::get_object_count();
      unit_error_state = true;
    }

    // Print any remaining leaks
    ::p3d_ecs::leak_detector::print_leaks();

    REQUIRE_EQ(::p3d_ecs::MessageManager::get_occurred_warnings(), {});

    if (::p3d_ecs::MessageManager::get_occurred_warnings().size() != 0) {
      std::cerr
        << termcolor::yellow
        << "ERROR: Uncatched warnings during test-run! The following warnings were not expected:"
        << termcolor::reset << std::endl;
      for (auto warning : ::p3d_ecs::MessageManager::take_occurred_warnings()) {
        std::cerr << static_cast<size_t>(warning) << " ";
        ++::UnittestStats::num_uncatched_warnings;
      }
      std::cerr << std::endl;
      unit_error_state = true;
    }

    if (unit_error_state == false) {
      std::cout << termcolor::green << "[  OK    ] " << termcolor::reset << std::endl;
    } else {
      std::cerr << termcolor::red << "[  FAIL  ]" << termcolor::grey << " " << file << ":" << line
                << termcolor::reset << endl;
    }
  }

  RequireWarningScoped::RequireWarningScoped(::std::initializer_list<::p3d_ecs::Warning> warnings)
    : _warnings(warnings.begin(), warnings.end()) {
    if (!REQUIRE_EQ(::p3d_ecs::MessageManager::get_occurred_warnings(), {})) {
      std::cout << termcolor::cyan << "  Trying to recover from previous failure."
                << termcolor::reset << std::endl;
      ::p3d_ecs::MessageManager::clear_occured_warnings();
    }
    std::cout << termcolor::cyan << "  Expecting " << _warnings.size()
              << " warning(s) to occur:" << termcolor::reset << endl;
  };

  RequireWarningScoped::~RequireWarningScoped() {
    if (!REQUIRE_EQ(::p3d_ecs::MessageManager::get_occurred_warnings(), _warnings)) {
      unit_error_state = true;
      std::cout << termcolor::cyan << "  Unexpected warnings, clearing anyway to keep flow going."
                << termcolor::reset << std::endl;
      ::p3d_ecs::MessageManager::clear_occured_warnings();
    } else {
      ::p3d_ecs::MessageManager::take_warnings(_warnings);
    }
  };
}
