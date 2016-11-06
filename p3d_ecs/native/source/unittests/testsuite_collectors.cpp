
#include "testsuite_collectors.h"

#include "movement_system.h"
#include "unittest.h"

#include <iostream>
#include <unordered_set>
#include <functional>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

void
testsuite_collectors()
{

  struct TestSystem : public SimpleEntitySystem<PhysicsComponent>
  {
    TestSystem(EntityManager* mgr)
      : SimpleEntitySystem<PhysicsComponent>(mgr){};
    virtual void process(float dt) override
    {

      processed_entities = 0;
      for (auto entity : get_entities()) {
        TC_REQUIRE_EQ(entity->has_component<PhysicsComponent>(), true);
        ++processed_entities;
      };
    }

    size_t processed_entities = 0;
  };

  BEGIN_TESTCASE("Collectors - Initial test")
  {

    TestSystem* sys = mgr->new_system<TestSystem>();

    Entity* entity = mgr->new_entity();
    entity->new_component<TransformComponent>();
    entity->new_component<PhysicsComponent>();

    mgr->process_changes();
    sys->process(1.0);

    TC_REQUIRE_EQ(sys->get_entities().size(), 1u);

    entity->remove();

    mgr->process_changes();

    TC_REQUIRE_EQ(sys->get_entities().size(), 0u);

    delete sys;
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Collectors - Add component after creation")
  {

    TestSystem* sys = mgr->new_system<TestSystem>();

    Entity* entity = mgr->new_entity();
    entity->new_component<TransformComponent>();

    mgr->process_changes();
    sys->process(1.0);

    TC_REQUIRE_EQ(sys->get_entities().size(), 0u);

    entity->new_component<PhysicsComponent>();
    mgr->process_changes();

    TC_REQUIRE_EQ(sys->get_entities().size(), 1u);

    mgr->process_changes();

    entity->remove();

    mgr->process_changes();

    TC_REQUIRE_EQ(sys->get_entities().size(), 0u);

    delete sys;
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Collectors - Add component and remove entity")
  {

    TestSystem* sys = mgr->new_system<TestSystem>();

    Entity* entity = mgr->new_entity();

    TC_REQUIRE_EQ(sys->get_entities().size(), 0u);
    mgr->process_changes();

    entity->new_component<PhysicsComponent>();

    sys->process(1.0);
    TC_REQUIRE_EQ(sys->processed_entities, 0);

    mgr->process_changes();

    sys->process(1.0);
    TC_REQUIRE_EQ(sys->processed_entities, 1);

    entity->remove();

    mgr->print_status();
    mgr->process_changes();

    sys->process(1.0);
    TC_REQUIRE_EQ(sys->processed_entities, 0);

    TC_REQUIRE_EQ(sys->get_entities().size(), 0u);

    delete sys;
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Collectors - Add and then remove component")
  {

    TestSystem* sys = mgr->new_system<TestSystem>();

    Entity* entity = mgr->new_entity();
    mgr->process_changes();

    entity->new_component<PhysicsComponent>();

    sys->process(1.0);
    TC_REQUIRE_EQ(sys->processed_entities, 0);

    mgr->process_changes();

    sys->process(1.0);
    TC_REQUIRE_EQ(sys->processed_entities, 1);

    entity->remove_component<PhysicsComponent>();

    sys->process(1.0);
    // Removal of components directly affects the list of
    // captured entities
    TC_REQUIRE_EQ(sys->processed_entities, 0);

    mgr->process_changes();

    sys->process(1.0);
    TC_REQUIRE_EQ(sys->processed_entities, 0);

    entity->remove();

    mgr->print_status();
    mgr->process_changes();

    TC_REQUIRE_EQ(sys->get_entities().size(), 0u);

    delete sys;
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Collectors - Remove entity while iterating")
  {

    struct FaultySystem : public SimpleEntitySystem<PhysicsComponent>
    {
      FaultySystem(EntityManager* mgr)
        : SimpleEntitySystem<PhysicsComponent>(mgr){};
      virtual void process(float dt) override
      {
        processed_entities = 0;
        for (auto entity : get_entities()) {
          TC_REQUIRE_EQ(entity->has_component<PhysicsComponent>(), true);
          ++processed_entities;
          if (processed_entities == 10)
            entity->remove();
        };
      };

      size_t processed_entities = 0;
    };

    FaultySystem* sys = mgr->new_system<FaultySystem>();
    TestSystem* sys2 = mgr->new_system<TestSystem>();

    std::vector<Entity*> entities;
    for (size_t i = 0; i < 10; ++i) {
      Entity* entity = mgr->new_entity();
      entity->new_component<PhysicsComponent>();
      entities.push_back(entity);
    }

    TC_REQUIRE_EQ(mgr->get_num_entities(), 10);

    mgr->process_changes();
    sys->process(1.0);
    sys2->process(1.0);
    TC_REQUIRE_EQ(sys->processed_entities, 10);
    TC_REQUIRE_EQ(sys2->processed_entities, 9);

    TC_REQUIRE_EQ(mgr->get_num_entities(), 10);

    mgr->process_changes(); // entity should be removed here
    TC_REQUIRE_EQ(mgr->get_num_entities(), 9);

    sys->process(1.0);
    sys2->process(1.0);

    TC_REQUIRE_EQ(sys->processed_entities, 9);
    TC_REQUIRE_EQ(sys2->processed_entities, 9);
    TC_REQUIRE_EQ(mgr->get_num_entities(), 9);

    entities.pop_back(); // last one is already removed
    for (auto entity : entities)
      entity->remove();

    mgr->process_changes();

    TC_REQUIRE_EQ(mgr->get_num_entities(), 0);
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Collectors - Remove same component while iterating")
  {

    struct FaultySystem : public SimpleEntitySystem<PhysicsComponent>
    {
      FaultySystem(EntityManager* mgr)
        : SimpleEntitySystem<PhysicsComponent>(mgr){};
      virtual void process(float dt) override
      {
        processed_entities = 0;
        for (auto entity : get_entities()) {
          TC_REQUIRE_EQ(entity->has_component<PhysicsComponent>(), true);
          ++processed_entities;
          if (processed_entities == 10)
            entity->remove_component<PhysicsComponent>();
        };
      };

      size_t processed_entities = 0;
    };

    FaultySystem* sys = mgr->new_system<FaultySystem>();
    TestSystem* sys2 = mgr->new_system<TestSystem>();

    std::vector<Entity*> entities;
    for (size_t i = 0; i < 10; ++i) {
      Entity* entity = mgr->new_entity();
      entity->new_component<PhysicsComponent>();
      entities.push_back(entity);
    }

    TC_REQUIRE_EQ(mgr->get_num_entities(), 10);

    mgr->process_changes();
    sys->process(1.0);
    sys2->process(1.0);
    TC_REQUIRE_EQ(sys->processed_entities, 10);
    TC_REQUIRE_EQ(sys2->processed_entities, 9);

    TC_REQUIRE_EQ(mgr->get_num_entities(), 10);

    mgr->process_changes(); // entity should be removed here
    TC_REQUIRE_EQ(mgr->get_num_entities(), 10);

    sys->process(1.0);
    sys2->process(1.0);

    TC_REQUIRE_EQ(sys->processed_entities, 9);
    TC_REQUIRE_EQ(sys2->processed_entities, 9);
    TC_REQUIRE_EQ(mgr->get_num_entities(), 10);

    for (auto entity : entities)
      entity->remove();

    mgr->process_changes();

    TC_REQUIRE_EQ(mgr->get_num_entities(), 0);
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Collectors - Add and Remove multiple times")
  {

    TestSystem* sys = mgr->new_system<TestSystem>();

    Entity* entity = mgr->new_entity();

    for (size_t i = 0; i < 10; ++i) {
      entity->new_component<PhysicsComponent>();
      entity->new_component<TransformComponent>();
      entity->remove_component<TransformComponent>();
      entity->remove_component<PhysicsComponent>();
    }

    TC_REQUIRE_EQ(entity->get_num_components(), 0);
    TC_REQUIRE_EQ(entity->get_component_mask(), 0);

    mgr->process_changes();
    sys->process(1.0);
    TC_REQUIRE_EQ(sys->processed_entities, 0);

    entity->new_component<PhysicsComponent>();
    entity->remove_component<PhysicsComponent>();

    mgr->process_changes();
    sys->process(1.0);
    TC_REQUIRE_EQ(sys->processed_entities, 0);

    entity->new_component<PhysicsComponent>();

    mgr->process_changes();
    sys->process(1.0);
    TC_REQUIRE_EQ(sys->processed_entities, 1);

    entity->remove();

    delete sys;
  }
  END_TESTCASE;
}
