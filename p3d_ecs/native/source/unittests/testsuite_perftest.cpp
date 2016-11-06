

#include "testsuite_perftest.h"

#include "unittest.h"

// Scales all iterations to make the perfest last longer, ideal for profiling
#define MULTIPLIER 1

// Simple system which modifies transform components
struct TestSystem_1 : public SimpleEntitySystem<TransformComponent>
{
  TestSystem_1(EntityManager* mgr)
    : SimpleEntitySystem<TransformComponent>(mgr){};
  virtual void process(float dt) override
  {
    for (auto entity : get_entities()) {
      entity->get_component<TransformComponent>().set_pos({ 1, 2, 3 });
    };
  }
};

void
testsuite_perftest()
{

  BEGIN_TESTCASE("Simple entity creation")
  {

    size_t iterations = 10000000 * MULTIPLIER;
    BEGIN_MEASURE("Create entities", iterations)
    {
      for (size_t i = 0; i < iterations; ++i) {
        Entity* entity = mgr->new_entity();
      }
    }
    END_MEASURE;

    BEGIN_MEASURE("First manager update", 1) { mgr->process_changes(); }
    END_MEASURE;

    BEGIN_MEASURE("Manager cleanup", 1) { mgr->reset(); }
    END_MEASURE;
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Entity creation with component")
  {

    size_t iterations = 5000000 * MULTIPLIER;
    BEGIN_MEASURE("Create entities", iterations)
    {
      for (size_t i = 0; i < iterations; ++i) {
        Entity* entity = mgr->new_entity();
        TransformComponent& transform =
          entity->new_component<TransformComponent>();
      }
    }
    END_MEASURE;

    BEGIN_MEASURE("First manager update", 1) { mgr->process_changes(); }
    END_MEASURE;

    mgr->reset();
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Entity::get_component benchmark")
  {
    Entity* entity = mgr->new_entity();
    entity->new_component<TransformComponent>();

    size_t iterations = 200000000 * MULTIPLIER;
    BEGIN_MEASURE("get_component<TransformComponent>", iterations)
    {
      for (size_t i = 0; i < iterations; ++i) {
        entity->get_component<TransformComponent>().set_pos({ 1, 2, 3 });
      }
    }
    END_MEASURE;

    entity->remove();
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Simple entity system benchmark")
  {

    TestSystem_1* sys = mgr->new_system<TestSystem_1>();

    size_t iterations = 10000000 * MULTIPLIER;
    for (size_t i = 0; i < iterations; ++i) {
      Entity* entity = mgr->new_entity();
      TransformComponent& transform =
        entity->new_component<TransformComponent>();
    }

    BEGIN_MEASURE("mgr->process_changes()", iterations)
    {
      mgr->process_changes();
    }
    END_MEASURE;

    BEGIN_MEASURE("system->process(dt)", iterations) { sys->process(1.0); }
    END_MEASURE;
  }
  END_TESTCASE;
}
