
#include "testsuite_entity.h"

#include "unittest.h"
#include "transform_component.h"
#include "physics_component.h"

void
testsuite_entity() {

  BEGIN_TESTCASE("Removing entity twice - expecting a warning!") {

    Entity* entity = mgr->new_entity();

    entity->remove(); // fine
    {
      REQUIRE_WARNING_SCOPED(ENT_0001);
      entity->remove(); // bad!
    }
  }
  END_TESTCASE;

  // bidirectional data equals
  auto data_equals_bd = [](Entity* a, Entity* b) {
    bool r_a = a->data_equals(b);
    bool r_b = b->data_equals(a);
    REQUIRE_EQ(r_a, r_b);
    return r_a;
  };

  BEGIN_TESTCASE("Testing Entity::data_equals - identity") {
    Entity* entity1 = mgr->new_entity();
    REQUIRE_TRUE(data_equals_bd(entity1, entity1));
    entity1->remove();
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Testing Entity::data_equals - empty components") {
    Entity* entity1 = mgr->new_entity();
    Entity* entity2 = mgr->new_entity();
    REQUIRE_TRUE(data_equals_bd(entity1, entity2));
    entity1->remove();
    entity2->remove();
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Testing Entity::data_equals - single empty component") {
    Entity* entity1 = mgr->new_entity();
    Entity* entity2 = mgr->new_entity();
    entity1->new_component<TransformComponent>();
    entity2->new_component<TransformComponent>();
    REQUIRE_TRUE(data_equals_bd(entity1, entity2));

    entity1->remove();
    entity2->remove();
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Testing Component::data_equals - same component") {
    Entity* entity1 = mgr->new_entity();
    Entity* entity2 = mgr->new_entity();
    TransformComponent& a = entity1->new_component<TransformComponent>();
    TransformComponent& b = entity2->new_component<TransformComponent>();
    REQUIRE_TRUE(data_equals_bd(entity1, entity2));
    REQUIRE_TRUE(a.data_equals(b));
    REQUIRE_TRUE(b.data_equals(a));
    REQUIRE_TRUE(a.data_equals(a));

    entity1->remove();
    entity2->remove();
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Testing Entity::data_equals - different mask") {
    Entity* entity1 = mgr->new_entity();
    Entity* entity2 = mgr->new_entity();
    entity1->new_component<TransformComponent>();
    REQUIRE_FALSE(data_equals_bd(entity1, entity2));

    entity1->remove();
    entity2->remove();
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Testing Entity::data_equals - single component") {
    Entity* entity1 = mgr->new_entity();
    Entity* entity2 = mgr->new_entity();
    TransformComponent& tr_1 = entity1->new_component<TransformComponent>();
    TransformComponent& tr_2 = entity2->new_component<TransformComponent>();
    REQUIRE_TRUE(data_equals_bd(entity1, entity2));

    tr_1.set_pos({ 1, 2, 3 });
    REQUIRE_FALSE(data_equals_bd(entity1, entity2));

    tr_2.set_pos({ 1, 2, 3 });
    REQUIRE_TRUE(data_equals_bd(entity1, entity2));

    entity1->remove();
    entity2->remove();
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Testing Entity::data_equals - remove component") {
    Entity* entity1 = mgr->new_entity();
    Entity* entity2 = mgr->new_entity();
    TransformComponent& tr_1 = entity1->new_component<TransformComponent>();
    TransformComponent& tr_2 = entity2->new_component<TransformComponent>();
    REQUIRE_TRUE(data_equals_bd(entity1, entity2));

    entity1->remove_component<TransformComponent>();
    REQUIRE_FALSE(data_equals_bd(entity1, entity2));

    entity2->remove_component<TransformComponent>();
    REQUIRE_TRUE(data_equals_bd(entity1, entity2));

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

    REQUIRE_TRUE(data_equals_bd(entity1, entity2));

    tr_1.set_pos({ 1, 2, 3 });
    REQUIRE_FALSE(data_equals_bd(entity1, entity2));

    tr_2.set_pos({ 1, 2, 3 });
    REQUIRE_TRUE(data_equals_bd(entity1, entity2));

    pc_1.set_mass(2.0);
    REQUIRE_FALSE(data_equals_bd(entity1, entity2));

    pc_2.set_mass(2.0);
    REQUIRE_TRUE(data_equals_bd(entity1, entity2));

    entity1->remove_component<TransformComponent>();
    REQUIRE_FALSE(data_equals_bd(entity1, entity2));

    entity1->remove_component<PhysicsComponent>();
    REQUIRE_FALSE(data_equals_bd(entity1, entity2));

    entity2->remove_component<TransformComponent>();
    REQUIRE_FALSE(data_equals_bd(entity1, entity2));

    entity2->remove_component<PhysicsComponent>();
    REQUIRE_TRUE(data_equals_bd(entity1, entity2));

    entity1->remove();
    entity2->remove();
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Testing Entity::data_equals - Transform component with children") {

    Entity* child_1 = mgr->new_entity();
    TransformComponent& ct_1 = child_1->new_component<TransformComponent>();
    Entity* child_2 = mgr->new_entity();
    TransformComponent& ct_2 = child_2->new_component<TransformComponent>();

    Entity* root_1 = mgr->new_entity();
    TransformComponent& tr_1 = root_1->new_component<TransformComponent>();
    Entity* root_2 = mgr->new_entity();
    TransformComponent& tr_2 = root_2->new_component<TransformComponent>();

    REQUIRE_TRUE(data_equals_bd(root_1, root_2));

    ct_1.set_parent(root_1);

    REQUIRE_FALSE(data_equals_bd(root_1, root_2));

    ct_2.set_parent(root_2);

    REQUIRE_FALSE(data_equals_bd(root_1, root_2));

    ct_1.set_parent(nullptr);
    ct_2.set_parent(nullptr);

    REQUIRE_TRUE(data_equals_bd(root_1, root_2));

    child_1->remove();
    child_2->remove();
    root_1->remove();
    root_2->remove();
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Entity component mask") {
    Entity* entity = mgr->new_entity();
    REQUIRE_EQ(entity->get_component_mask(), 0u);

    entity->new_component<TransformComponent>();
    REQUIRE_EQ(entity->get_component_mask(),
               Component::to_bitmask(Component::extract_id<TransformComponent>()));

    entity->remove_component<TransformComponent>();
    REQUIRE_EQ(entity->get_component_mask(), 0u);

    entity->remove();
    REQUIRE_EQ(entity->get_component_mask(), 0u);
    mgr->process_changes();
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Entity component mask - Remove registered with components left") {
    Entity* entity = mgr->new_entity();
    REQUIRE_EQ(entity->get_component_mask(), 0u);
    mgr->process_changes();

    entity->new_component<TransformComponent>();
    REQUIRE_EQ(entity->get_component_mask(),
               Component::to_bitmask(Component::extract_id<TransformComponent>()));

    entity->remove();
    REQUIRE_EQ(entity->get_component_mask(), 0u);
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Entity component mask - Remove registered with registered components left") {
    Entity* entity = mgr->new_entity();
    REQUIRE_EQ(entity->get_component_mask(), 0u);
    mgr->process_changes();

    entity->new_component<TransformComponent>();
    mgr->process_changes();
    REQUIRE_EQ(entity->get_component_mask(),
               Component::to_bitmask(Component::extract_id<TransformComponent>()));

    entity->remove();
    REQUIRE_EQ(entity->get_component_mask(), 0u);
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Testing entity component mask - new/remove component") {

    Entity* entity = mgr->new_entity();
    Component::bitmask_t transform_bit =
      Component::to_bitmask(Component::extract_id<TransformComponent>());
    Component::bitmask_t physics_bit =
      Component::to_bitmask(Component::extract_id<PhysicsComponent>());
    REQUIRE_EQ(entity->get_component_mask(), 0u);

    entity->new_component<TransformComponent>();
    REQUIRE_EQ(entity->get_component_mask(), transform_bit);

    entity->new_component<PhysicsComponent>();
    REQUIRE_EQ(entity->get_component_mask(), transform_bit | physics_bit);

    entity->remove_component<TransformComponent>();
    REQUIRE_EQ(entity->get_component_mask(), physics_bit);

    entity->remove_component<PhysicsComponent>();
    REQUIRE_EQ(entity->get_component_mask(), 0u);

    entity->remove();
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Adding component type twice") {

    Entity* entity = mgr->new_entity();
    TransformComponent& comp = entity->new_component<TransformComponent>();
    {
      REQUIRE_WARNING_SCOPED(ENT_0004);
      TransformComponent& comp2 = entity->new_component<TransformComponent>();

      REQUIRE_EQ(&comp, &comp2);
    }
    entity->remove();
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Add component to registered and for deletion flagged entity") {
    Entity* entity = mgr->new_entity();
    entity->new_component<PhysicsComponent>();

    mgr->process_changes();
    entity->remove();

    {
      REQUIRE_WARNING_SCOPED(ENT_0005);
      entity->new_component<TransformComponent>();
    }
  }
  END_TESTCASE;
};
