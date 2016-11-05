

#include "perftest_1.h"

#include "unittest.h"

#define MULTIPLIER 1

struct TestSystem_1 : public SimpleEntitySystem<TransformComponent> {
  TestSystem_1(EntityManager *mgr)
      : SimpleEntitySystem<TransformComponent>(mgr){};
  virtual void process(float dt) override {
    for (auto entity : get_entities()) {
      entity->get_component<TransformComponent>().set_pos({1, 2, 3});
    };
  }
};

void perftest_entities() {

  general_testsuite("Simple entity creation", [](EntityManager *mgr) {

    size_t iterations = 10000000 * MULTIPLIER;
    measure_time("Create entities",
                 [&]() {
                   for (size_t i = 0; i < iterations; ++i) {
                     Entity *entity = mgr->new_entity();
                   }
                 },
                 iterations);

    measure_time("First manager update", [&]() { mgr->process_changes(); });

    measure_time("Manager cleanup", [&]() { mgr->reset(); });

  });

  general_testsuite("Entity creation with component", [](EntityManager *mgr) {

    size_t iterations = 5000000 * MULTIPLIER;
    measure_time("Create entities",
                 [&]() {
                   for (size_t i = 0; i < iterations; ++i) {
                     Entity *entity = mgr->new_entity();
                     TransformComponent &transform =
                         entity->new_component<TransformComponent>();
                   }
                 },
                 iterations);

    measure_time("First manager update", [&]() { mgr->process_changes(); });

    mgr->reset();
  });

  general_testsuite("Entity::get_component benchmark", [](EntityManager *mgr) {
    Entity *entity = mgr->new_entity();
    entity->new_component<TransformComponent>();

    size_t iterations = 200000000 * MULTIPLIER;
    measure_time(
        "get_component<TransformComponent>",
        [&]() {
          for (size_t i = 0; i < iterations; ++i) {
            entity->get_component<TransformComponent>().set_pos({1, 2, 3});
          }
        },
        iterations);

    entity->remove();
  });

  general_testsuite("Simple entity system benchmark", [](EntityManager *mgr) {

    TestSystem_1 *sys = mgr->new_system<TestSystem_1>();

    size_t iterations = 10000000 * MULTIPLIER;
    for (size_t i = 0; i < iterations; ++i) {
      Entity *entity = mgr->new_entity();
      TransformComponent &transform =
          entity->new_component<TransformComponent>();
    }

    measure_time("mgr->process_changes()", [&]() { mgr->process_changes(); },
                 iterations);

    measure_time("system->process(dt)", [&]() { sys->process(1.0); },
                 iterations);

  });
}
