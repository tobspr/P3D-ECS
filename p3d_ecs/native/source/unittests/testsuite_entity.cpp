
#include "testsuite_entity.h"

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
testsuite_entity() {

  BEGIN_TESTCASE("Removing entity twice - expecting a warning!") {

    Entity* entity = mgr->new_entity();

    entity->remove(); // fine
    entity->remove(); // bad!
  }
  END_TESTCASE;

  // bidirectional data equals
  auto data_equals_bd = [](Entity* a, Entity* b) {
    bool r_a = a->data_equals(b);
    bool r_b = b->data_equals(a);
    TC_REQUIRE_EQ(r_a, r_b);
    return r_a;
  };

  BEGIN_TESTCASE("Testing Entity::data_equals - identity") {
    Entity* entity1 = mgr->new_entity();
    TC_REQUIRE_TRUE(data_equals_bd(entity1, entity1));
    entity1->remove();
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Testing Entity::data_equals - empty components") {
    Entity* entity1 = mgr->new_entity();
    Entity* entity2 = mgr->new_entity();
    TC_REQUIRE_TRUE(data_equals_bd(entity1, entity2));
    entity1->remove();
    entity2->remove();
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Testing Entity::data_equals - single empty component") {
    Entity* entity1 = mgr->new_entity();
    Entity* entity2 = mgr->new_entity();
    entity1->new_component<TransformComponent>();
    entity2->new_component<TransformComponent>();
    TC_REQUIRE_TRUE(data_equals_bd(entity1, entity2));

    entity1->remove();
    entity2->remove();
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Testing Component::data_equals - same component") {
    Entity* entity1 = mgr->new_entity();
    Entity* entity2 = mgr->new_entity();
    TransformComponent& a = entity1->new_component<TransformComponent>();
    TransformComponent& b = entity2->new_component<TransformComponent>();
    TC_REQUIRE_TRUE(data_equals_bd(entity1, entity2));
    TC_REQUIRE_TRUE(a.data_equals(b));
    TC_REQUIRE_TRUE(b.data_equals(a));
    TC_REQUIRE_TRUE(a.data_equals(a));

    entity1->remove();
    entity2->remove();
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Testing Entity::data_equals - different mask") {
    Entity* entity1 = mgr->new_entity();
    Entity* entity2 = mgr->new_entity();
    entity1->new_component<TransformComponent>();
    TC_REQUIRE_FALSE(data_equals_bd(entity1, entity2));

    entity1->remove();
    entity2->remove();
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Testing Entity::data_equals - single component") {
    Entity* entity1 = mgr->new_entity();
    Entity* entity2 = mgr->new_entity();
    TransformComponent& tr_1 = entity1->new_component<TransformComponent>();
    TransformComponent& tr_2 = entity2->new_component<TransformComponent>();
    TC_REQUIRE_TRUE(data_equals_bd(entity1, entity2));

    tr_1.set_pos({ 1, 2, 3 });
    TC_REQUIRE_FALSE(data_equals_bd(entity1, entity2));

    tr_2.set_pos({ 1, 2, 3 });
    TC_REQUIRE_TRUE(data_equals_bd(entity1, entity2));

    entity1->remove();
    entity2->remove();
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Testing Entity::data_equals - remove component") {
    Entity* entity1 = mgr->new_entity();
    Entity* entity2 = mgr->new_entity();
    TransformComponent& tr_1 = entity1->new_component<TransformComponent>();
    TransformComponent& tr_2 = entity2->new_component<TransformComponent>();
    TC_REQUIRE_TRUE(data_equals_bd(entity1, entity2));

    entity1->remove_component<TransformComponent>();
    TC_REQUIRE_FALSE(data_equals_bd(entity1, entity2));

    entity2->remove_component<TransformComponent>();
    TC_REQUIRE_TRUE(data_equals_bd(entity1, entity2));

    entity1->remove();
    entity2->remove();
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Testing Entity::data_equals - multi component with different order") {
    Entity* entity1 = mgr->new_entity();
    Entity* entity2 = mgr->new_entity();
    TransformComponent& tr_1 = entity1->new_component<TransformComponent>();
    PhysicsComponent& pc_1 = entity1->new_component<PhysicsComponent>();

    PhysicsComponent& pc_2 = entity2->new_component<PhysicsComponent>();
    TransformComponent& tr_2 = entity2->new_component<TransformComponent>();

    TC_REQUIRE_TRUE(data_equals_bd(entity1, entity2));

    tr_1.set_pos({ 1, 2, 3 });
    TC_REQUIRE_FALSE(data_equals_bd(entity1, entity2));

    tr_2.set_pos({ 1, 2, 3 });
    TC_REQUIRE_TRUE(data_equals_bd(entity1, entity2));

    pc_1.set_mass(2.0);
    TC_REQUIRE_FALSE(data_equals_bd(entity1, entity2));

    pc_2.set_mass(2.0);
    TC_REQUIRE_TRUE(data_equals_bd(entity1, entity2));

    entity1->remove_component<TransformComponent>();
    TC_REQUIRE_FALSE(data_equals_bd(entity1, entity2));

    entity1->remove_component<PhysicsComponent>();
    TC_REQUIRE_FALSE(data_equals_bd(entity1, entity2));

    entity2->remove_component<TransformComponent>();
    TC_REQUIRE_FALSE(data_equals_bd(entity1, entity2));

    entity2->remove_component<PhysicsComponent>();
    TC_REQUIRE_TRUE(data_equals_bd(entity1, entity2));

    entity1->remove();
    entity2->remove();
  }
  END_TESTCASE;
};
