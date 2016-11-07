
#include "testsuite_generic.h"

#include "movement_system.h"
#include "unittest.h"
#include "entity.h"

#include <iostream>
#include <unordered_set>
#include <functional>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

void
testsuite_generic() {

  BEGIN_TESTCASE("Re-Parenting entity with existing parent") {

    Entity* root_1 = mgr->new_entity();
    Entity* root_2 = mgr->new_entity();
    Entity* child = mgr->new_entity();

    TransformComponent& root_1_transform = root_1->new_component<TransformComponent>();
    TransformComponent& root_2_transform = root_2->new_component<TransformComponent>();

    TransformComponent& transform = child->new_component<TransformComponent>();
    transform.set_parent(root_1);

    TC_REQUIRE_EQ(root_1_transform.get_children().size(), 1);
    TC_REQUIRE_EQ(root_2_transform.get_children().size(), 0);

    transform.set_parent(root_2);
    TC_REQUIRE_EQ(root_1_transform.get_children().size(), 0);
    TC_REQUIRE_EQ(root_2_transform.get_children().size(), 1);

    transform.set_parent(nullptr);
    TC_REQUIRE_EQ(root_1_transform.get_children().size(), 0);
    TC_REQUIRE_EQ(root_2_transform.get_children().size(), 0);
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

  } END_TESTCASE;
  
  BEGIN_TESTCASE("Testing EntityManager::find_entity with empty id - Expecting a warning!") {
    
    TC_REQUIRE_EQ(mgr->find_entity(Entity::EMPTY_ID), nullptr);

  } END_TESTCASE;


  BEGIN_TESTCASE("Testing EntityManager::find_entity with new entity") {
    Entity* entity = mgr->new_entity();
    TC_REQUIRE_EQ(mgr->find_entity(entity->get_id()), entity);
    entity->remove();
  } END_TESTCASE;


  BEGIN_TESTCASE("Testing EntityManager::find_entity with registered entity") {
    Entity* entity = mgr->new_entity();
    mgr->process_changes();
    TC_REQUIRE_EQ(mgr->find_entity(entity->get_id()), entity);
    entity->remove();
  } END_TESTCASE;

};
