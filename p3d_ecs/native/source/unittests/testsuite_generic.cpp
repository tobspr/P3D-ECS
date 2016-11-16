
#include "testsuite_generic.h"

#include "unittest.h"
#include "physics_component.h"
#include "transform_component.h"

void
testsuite_generic() {

  BEGIN_TESTCASE("Testing unittests") {

    REQUIRE_TRUE(true);
    REQUIRE_TRUE(1);
    REQUIRE_TRUE('\1');
    REQUIRE_EQ(3, 3u); // Different datatypes

    REQUIRE_FALSE(false);
    REQUIRE_FALSE(0);
    REQUIRE_FALSE('\0');
    REQUIRE_EQ(1, 1);
    REQUIRE_EQ(-1, -1);

    REQUIRE_PTR((reinterpret_cast<int*>(123u)));

    std::vector<int> a = { 1, 2, 3 };
    std::vector<int> b = { 1, 2, 3 };
    REQUIRE_EQ(a, b);
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Testing size of builtin types") {

    REQUIRE_TRUE(sizeof(Entity::id_t) >= 8);
    REQUIRE_TRUE(sizeof(Component::bitmask_t) >= 8);
    REQUIRE_TRUE(sizeof(Component::id_t) >= 1);
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Re-Parenting entity with existing parent") {

    Entity* root_1 = mgr->new_entity();
    Entity* root_2 = mgr->new_entity();
    Entity* child = mgr->new_entity();

    TransformComponent& root_1_transform = root_1->new_component<TransformComponent>();
    TransformComponent& root_2_transform = root_2->new_component<TransformComponent>();

    TransformComponent& transform = child->new_component<TransformComponent>();
    transform.set_parent(root_1);

    REQUIRE_EQ(root_1_transform.get_children().size(), 1);
    REQUIRE_EQ(root_2_transform.get_children().size(), 0);

    transform.set_parent(root_2);
    REQUIRE_EQ(root_1_transform.get_children().size(), 0);
    REQUIRE_EQ(root_2_transform.get_children().size(), 1);

    transform.set_parent(nullptr);
    REQUIRE_EQ(root_1_transform.get_children().size(), 0);
    REQUIRE_EQ(root_2_transform.get_children().size(), 0);
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Creating two parallel entity managers") {

    Entity* created_1 = mgr->new_entity();
    created_1->new_component<TransformComponent>();

    EntityManager* mgr2 = new EntityManager();
    Entity* created_2 = mgr2->new_entity();
    created_2->new_component<TransformComponent>();

    created_2->remove();
    mgr2->process_changes();
    mgr2->reset();
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Testing EntityManager::reset") {

    EntityManager* mgr2 = new EntityManager();
    Entity* entity_reg = mgr2->new_entity();
    Entity* entity_reg_del = mgr2->new_entity();
    mgr2->process_changes();

    Entity* entity_unreg = mgr2->new_entity();
    Entity* entity_del = mgr2->new_entity();
    entity_del->remove();
    entity_reg_del->remove();

    mgr2->reset();
    // No leaks should occur now
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Testing EntityManager::find_entity with empty id - Expecting a warning!") {
    {
      REQUIRE_WARNING_SCOPED(EM_0001);
      REQUIRE_EQ(mgr->find_entity(Entity::EMPTY_ID), nullptr);
    }
  }
  END_TESTCASE;st


  BEGIN_TESTCASE("Testing EntityManager::reset") {
    Entity* entity_reg = mgr->new_entity();
    Entity* entity_reg_del = mgr->new_entity();
    mgr->process_changes();
    Entity* entity_unreg = mgr->new_entity();
    Entity* entity_del = mgr->new_entity();
    REQUIRE_EQ(mgr->get_num_entities(), 4u);
    entity_del->remove();
    REQUIRE_EQ(mgr->get_num_entities(), 3u);

    mgr->reset();
    REQUIRE_EQ(mgr->get_num_entities(), 0u);
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Testing EntityManager::find_entity with new entity") {
    Entity* entity = mgr->new_entity();
    REQUIRE_EQ(mgr->find_entity(entity->get_id()), entity);
    entity->remove();
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Testing EntityManager::find_entity with registered entity") {
    Entity* entity = mgr->new_entity();
    mgr->process_changes();
    REQUIRE_EQ(mgr->find_entity(entity->get_id()), entity);
    entity->remove();
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Testing EntityManager::find_entity(UUID) - Unregistered entity") {

    Entity* entity = mgr->new_entity();
    REQUIRE_EQ(mgr->find_entity(entity->get_uuid()), entity);
    UUID u = UUID::generate_from_string(entity->get_uuid().c_str());
    entity->remove();
    mgr->process_changes();

    {
      REQUIRE_WARNING_SCOPED(EM_0005);
      REQUIRE_EQ(mgr->find_entity(u), nullptr);
    }
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Testing EntityManager::find_entity(UUID) - Registered entity") {

    Entity* entity = mgr->new_entity();
    mgr->process_changes();
    REQUIRE_EQ(mgr->find_entity(entity->get_uuid()), entity);
    UUID u = UUID::generate_from_string(entity->get_uuid().c_str());
    entity->remove();

    mgr->process_changes();
    {
      REQUIRE_WARNING_SCOPED(EM_0005);
      REQUIRE_EQ(mgr->find_entity(u), nullptr);
    }
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Testing EntityManager::find_entity(UUID) - Find by copied uuid") {

    Entity* entity = mgr->new_entity();
    mgr->process_changes();
    REQUIRE_EQ(mgr->find_entity(entity->get_uuid()), entity);

    UUID u = UUID::generate_from_string(entity->get_uuid().c_str());
    REQUIRE_EQ(mgr->find_entity(u), entity);
    entity->remove();
    mgr->process_changes();
    {
      REQUIRE_WARNING_SCOPED(EM_0005);
      REQUIRE_EQ(mgr->find_entity(u), nullptr);
    }
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Testing EntityManager::find_entity(UUID) - Find nonexistent uuid") {
    UUID nonexistent = UUID::generate();

    {
      REQUIRE_WARNING_SCOPED(EM_0005);
      REQUIRE_EQ(mgr->find_entity(nonexistent), nullptr);
    }
  }
  END_TESTCASE;

  BEGIN_TESTCASE(
    "Testing EntityManager::find_entity(UUID) - Find registered entity flagged for deletion") {

    Entity* entity = mgr->new_entity();
    mgr->process_changes();
    entity->remove();

    {
      REQUIRE_WARNING_SCOPED(EM_0004);
      REQUIRE_EQ(mgr->find_entity(entity->get_uuid()), entity);
    }

    UUID u = UUID::generate_from_string(entity->get_uuid().c_str());
    {
      REQUIRE_WARNING_SCOPED(EM_0004);
      REQUIRE_EQ(mgr->find_entity(u), entity);
    }

    mgr->process_changes();
    {
      REQUIRE_WARNING_SCOPED(EM_0005);
      REQUIRE_EQ(mgr->find_entity(u), nullptr);
    }
  }
  END_TESTCASE;
};
