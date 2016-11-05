

#include "unittest.h"

#include <chrono>

void write_tc_log(const string &msg) {
#ifndef PROFILING
  ofstream outfile("test-output.txt", ios_base::app);
  outfile << msg;
#endif
};

void measure_time(const string &desc, size_t num_iterations,
                  function<void()> method) {
  string iteration_desc = std::to_string(num_iterations);
  if (num_iterations >= 1000000) {
    iteration_desc =
        std::to_string((size_t)((float)num_iterations / 1000000.0)) + " mio.";
  }

  TC_STATUS("Start time measure for operation '"
            << desc << "' with " << iteration_desc << " iterations.");
  auto begin = chrono::high_resolution_clock::now();
  method();
  auto end = chrono::high_resolution_clock::now();

  size_t ms =
      chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
  TC_STATUS("Measurement done in " << ms << " ms");

  if (num_iterations > 1) {
    float ms_per_iteration = (float)ms / (float)num_iterations * 1000.0;
    TC_STATUS(" -> That is " << ms_per_iteration << " ns per iteration");
  }

  write_tc_log("Measure: " + desc + " (" + std::to_string(num_iterations) +
               ") took " + std::to_string(ms) + " ms.\n");
}

void run_testcase(const string &name, function<void(EntityManager *)> inner) {
  // Make sure we don't reach the maximum id
  Entity::reset_id_pool();

  // Reset memory pools
  MemoryPool<Entity>::reset();
  MemoryPool<TransformComponent>::reset();
  MemoryPool<PhysicsComponent>::reset();

  write_tc_log("Testcase: " + name + "\n");
  ECS_RESET_LEAKS();
#ifndef PROFILING
  cout << "\n\n\n=========== Testcase: " << name << " ===========" << endl;
#endif

  EntityManager *mgr = new EntityManager();
  inner(mgr);

  TC_STATUS("Testsuite done, cleaning up.");
  mgr->print_status();
  mgr->reset();

  TC_REQUIRE_EQ(mgr->get_num_entities(), 0);
  ECS_PRINT_LEAKS();

  delete mgr;

  write_tc_log("Done.\n");
}
