
#include "testsuite_entityref.h"

#include "unittest.h"
#include "entity_ref.h"

#include <iostream>
#include <functional>
#include <string>
#include <fstream>
#include <sstream>
#include <initializer_list>
using namespace std;

void testsuite_entityref() {

  auto validate_empty_ref = [&](EntityManager *mgr, EntityRef &ref) {
    TC_REQUIRE_EQ(ref.get_id(), Entity::EMPTY_ID);
    TC_REQUIRE_EQ(ref.get_ptr(mgr), nullptr);
    TC_REQUIRE_TRUE(ref.is_empty());
    TC_REQUIRE_EQ(ref.has_access(), false);
    TC_REQUIRE_TRUE(ref.fill_ptr(mgr));
    TC_REQUIRE_EQ(ref.operator->(), nullptr);
    TC_REQUIRE_EQ((Entity *)ref, nullptr);
  };

  auto validate_ref = [&](EntityManager *mgr, EntityRef &ref,
                          Entity *expected) {
    TC_REQUIRE_EQ(ref.get_id(), expected->get_id());
    TC_REQUIRE_EQ(ref.get_ptr(mgr), expected);
    TC_REQUIRE_EQ(ref.is_empty(), false);
    TC_REQUIRE_TRUE(ref.has_access());
    TC_REQUIRE_TRUE(ref.fill_ptr(mgr));
    TC_REQUIRE_EQ(ref.operator->(), expected);
    TC_REQUIRE_EQ((Entity *)ref, expected);
  };

  auto validate_ref_id_only = [&](EntityManager *mgr, EntityRef &ref,
                                  Entity *expected) {
    TC_REQUIRE_EQ(ref.get_id(), expected->get_id());
    TC_REQUIRE_EQ(ref.is_empty(), false);
    TC_REQUIRE_EQ(ref.has_access(), false);
    TC_REQUIRE_EQ(ref.operator->(), nullptr);
    TC_REQUIRE_EQ((Entity *)ref, nullptr);
  };

  BEGIN_TESTCASE("Empty ref test: EntityRef()") {
    EntityRef ref;
    validate_empty_ref(mgr, ref);
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Empty ref test: EntityRef(Entity::id_t)") {
    EntityRef ref(Entity::EMPTY_ID);
    validate_empty_ref(mgr, ref);
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Empty ref test: EntityRef(Entity*)") {
    EntityRef ref(nullptr);
    validate_empty_ref(mgr, ref);
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Simple ref test w/o register") {
    Entity *entity = mgr->new_entity();
    EntityRef ref(entity);
    validate_ref(mgr, ref, entity);

    entity->remove();
    // remove is processed directly, because the entity has not been registered
    // yet
    validate_empty_ref(mgr, ref);
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Simple ref test w/ register") {
    Entity *entity = mgr->new_entity();
    EntityRef ref(entity);
    validate_ref(mgr, ref, entity);

    mgr->process_changes();

    validate_ref(mgr, ref, entity);

    entity->remove();
    // remove is not processed directly, because the entity has been registered
    validate_ref(mgr, ref, entity);

    mgr->process_changes();

    validate_empty_ref(mgr, ref);
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Test delayed pointer expansion - New Entity") {
    Entity *entity = mgr->new_entity();
    EntityRef ref;

    ref.reset(entity->get_id());
    validate_ref_id_only(mgr, ref, entity);
    ref.fill_ptr(mgr);
    validate_ref(mgr, ref, entity);

    entity->remove();
    validate_empty_ref(mgr, ref);
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Test delayed pointer expansion - Registered Entity") {
    Entity *entity = mgr->new_entity();
    EntityRef ref;

    mgr->process_changes();

    ref.reset(entity->get_id());
    validate_ref_id_only(mgr, ref, entity);

    ref.fill_ptr(mgr);
    validate_ref(mgr, ref, entity);

    entity->remove();
    validate_ref(mgr, ref, entity);

    mgr->process_changes();
    validate_empty_ref(mgr, ref);
  }
  END_TESTCASE;

  enum class Operation {
    CreateEntity1,
    MgrProcessChanges,
    CreateRefFromEmpty,
    CreateRefFromNullptr,
    CreateRefFromEmptyId,
    CreateRefFromId,
    CreateRefFromEntityPtr,

    CreateEntity2,

    ResetRefFromEmpty,
    ResetRefFromNullptr,
    ResetRefFromEmptyId,
    ResetRefFromId,
    ResetRefFromEntityPtr,

    RefGetPtr,
    RefFillPtr,

    RemoveEntity1,
    RemoveEntity2,
    ValidateRefEmpty,
    ValidateRefToEntity2,
    ValidateRefToEntity2IdOnly
  };

  // see p3d_ecs/unittest-helpers/generate_testcase_entityref.py
  vector<vector<Operation>> testcases = {
      // Testcase 1, i=empty, r=empty, i=0
      {Operation::CreateRefFromEmpty, Operation::ResetRefFromEmpty,
       Operation::ValidateRefEmpty, Operation::ValidateRefEmpty},
      // Testcase 2, i=empty, r=empty, i=1
      {Operation::CreateRefFromEmpty, Operation::ResetRefFromEmpty,
       Operation::ValidateRefEmpty, Operation::ValidateRefEmpty},
      // Testcase 3, i=empty, r=empty, i=2
      {Operation::CreateRefFromEmpty, Operation::ResetRefFromEmpty,
       Operation::ValidateRefEmpty, Operation::ValidateRefEmpty},
      // Testcase 4, i=empty, r=empty, i=3
      {Operation::CreateRefFromEmpty, Operation::ResetRefFromEmpty,
       Operation::ValidateRefEmpty, Operation::ValidateRefEmpty},
      // Testcase 5, i=empty, r=nullptr, i=0
      {Operation::CreateRefFromEmpty, Operation::ResetRefFromNullptr,
       Operation::ValidateRefEmpty, Operation::ValidateRefEmpty},
      // Testcase 6, i=empty, r=nullptr, i=1
      {Operation::CreateRefFromEmpty, Operation::ResetRefFromNullptr,
       Operation::ValidateRefEmpty, Operation::ValidateRefEmpty},
      // Testcase 7, i=empty, r=nullptr, i=2
      {Operation::CreateRefFromEmpty, Operation::ResetRefFromNullptr,
       Operation::ValidateRefEmpty, Operation::ValidateRefEmpty},
      // Testcase 8, i=empty, r=nullptr, i=3
      {Operation::CreateRefFromEmpty, Operation::ResetRefFromNullptr,
       Operation::ValidateRefEmpty, Operation::ValidateRefEmpty},
      // Testcase 9, i=empty, r=empty_id, i=0
      {Operation::CreateRefFromEmpty, Operation::ResetRefFromEmptyId,
       Operation::ValidateRefEmpty, Operation::ValidateRefEmpty},
      // Testcase 10, i=empty, r=empty_id, i=1
      {Operation::CreateRefFromEmpty, Operation::ResetRefFromEmptyId,
       Operation::ValidateRefEmpty, Operation::ValidateRefEmpty},
      // Testcase 11, i=empty, r=empty_id, i=2
      {Operation::CreateRefFromEmpty, Operation::ResetRefFromEmptyId,
       Operation::ValidateRefEmpty, Operation::ValidateRefEmpty},
      // Testcase 12, i=empty, r=empty_id, i=3
      {Operation::CreateRefFromEmpty, Operation::ResetRefFromEmptyId,
       Operation::ValidateRefEmpty, Operation::ValidateRefEmpty},
      // Testcase 13, i=empty, r=id, i=0
      {Operation::CreateRefFromEmpty, Operation::CreateEntity2,
       Operation::ResetRefFromId, Operation::RefGetPtr,
       Operation::ValidateRefToEntity2, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity2, Operation::ValidateRefEmpty},
      // Testcase 14, i=empty, r=id, i=1
      {Operation::CreateRefFromEmpty, Operation::CreateEntity2,
       Operation::ResetRefFromId, Operation::RefFillPtr,
       Operation::ValidateRefToEntity2, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity2, Operation::ValidateRefEmpty},
      // Testcase 15, i=empty, r=id, i=2
      {Operation::CreateRefFromEmpty, Operation::CreateEntity2,
       Operation::ResetRefFromId, Operation::ValidateRefToEntity2IdOnly,
       Operation::RemoveEntity2, Operation::ValidateRefToEntity2IdOnly},
      // Testcase 16, i=empty, r=id, i=3
      {Operation::CreateRefFromEmpty, Operation::CreateEntity2,
       Operation::ResetRefFromId, Operation::ValidateRefToEntity2IdOnly,
       Operation::RemoveEntity2, Operation::ValidateRefToEntity2IdOnly},
      // Testcase 17, i=empty, r=reg_id, i=0
      {Operation::CreateRefFromEmpty, Operation::CreateEntity2,
       Operation::MgrProcessChanges, Operation::ResetRefFromId,
       Operation::RefGetPtr, Operation::ValidateRefToEntity2,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2, Operation::MgrProcessChanges,
       Operation::ValidateRefEmpty},
      // Testcase 18, i=empty, r=reg_id, i=1
      {Operation::CreateRefFromEmpty, Operation::CreateEntity2,
       Operation::MgrProcessChanges, Operation::ResetRefFromId,
       Operation::RefFillPtr, Operation::ValidateRefToEntity2,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2, Operation::MgrProcessChanges,
       Operation::ValidateRefEmpty},
      // Testcase 19, i=empty, r=reg_id, i=2
      {Operation::CreateRefFromEmpty, Operation::CreateEntity2,
       Operation::MgrProcessChanges, Operation::ResetRefFromId,
       Operation::ValidateRefToEntity2IdOnly, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2IdOnly},
      // Testcase 20, i=empty, r=reg_id, i=3
      {Operation::CreateRefFromEmpty, Operation::CreateEntity2,
       Operation::MgrProcessChanges, Operation::ResetRefFromId,
       Operation::ValidateRefToEntity2IdOnly, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2IdOnly},
      // Testcase 21, i=empty, r=entity, i=0
      {Operation::CreateRefFromEmpty, Operation::CreateEntity2,
       Operation::ResetRefFromEntityPtr, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity2, Operation::ValidateRefEmpty},
      // Testcase 22, i=empty, r=entity, i=1
      {Operation::CreateRefFromEmpty, Operation::CreateEntity2,
       Operation::ResetRefFromEntityPtr, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity2, Operation::ValidateRefEmpty},
      // Testcase 23, i=empty, r=entity, i=2
      {Operation::CreateRefFromEmpty, Operation::CreateEntity2,
       Operation::ResetRefFromEntityPtr, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity2, Operation::ValidateRefEmpty},
      // Testcase 24, i=empty, r=entity, i=3
      {Operation::CreateRefFromEmpty, Operation::CreateEntity2,
       Operation::ResetRefFromEntityPtr, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity2, Operation::ValidateRefEmpty},
      // Testcase 25, i=empty, r=reg_entity, i=0
      {Operation::CreateRefFromEmpty, Operation::CreateEntity2,
       Operation::MgrProcessChanges, Operation::ResetRefFromEntityPtr,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2, Operation::MgrProcessChanges,
       Operation::ValidateRefEmpty},
      // Testcase 26, i=empty, r=reg_entity, i=1
      {Operation::CreateRefFromEmpty, Operation::CreateEntity2,
       Operation::MgrProcessChanges, Operation::ResetRefFromEntityPtr,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2, Operation::MgrProcessChanges,
       Operation::ValidateRefEmpty},
      // Testcase 27, i=empty, r=reg_entity, i=2
      {Operation::CreateRefFromEmpty, Operation::CreateEntity2,
       Operation::MgrProcessChanges, Operation::ResetRefFromEntityPtr,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2, Operation::MgrProcessChanges,
       Operation::ValidateRefEmpty},
      // Testcase 28, i=empty, r=reg_entity, i=3
      {Operation::CreateRefFromEmpty, Operation::CreateEntity2,
       Operation::MgrProcessChanges, Operation::ResetRefFromEntityPtr,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2, Operation::MgrProcessChanges,
       Operation::ValidateRefEmpty},
      // Testcase 29, i=nullptr, r=empty, i=0
      {Operation::CreateRefFromNullptr, Operation::ResetRefFromEmpty,
       Operation::ValidateRefEmpty, Operation::ValidateRefEmpty},
      // Testcase 30, i=nullptr, r=empty, i=1
      {Operation::CreateRefFromNullptr, Operation::ResetRefFromEmpty,
       Operation::ValidateRefEmpty, Operation::ValidateRefEmpty},
      // Testcase 31, i=nullptr, r=empty, i=2
      {Operation::CreateRefFromNullptr, Operation::ResetRefFromEmpty,
       Operation::ValidateRefEmpty, Operation::ValidateRefEmpty},
      // Testcase 32, i=nullptr, r=empty, i=3
      {Operation::CreateRefFromNullptr, Operation::ResetRefFromEmpty,
       Operation::ValidateRefEmpty, Operation::ValidateRefEmpty},
      // Testcase 33, i=nullptr, r=nullptr, i=0
      {Operation::CreateRefFromNullptr, Operation::ResetRefFromNullptr,
       Operation::ValidateRefEmpty, Operation::ValidateRefEmpty},
      // Testcase 34, i=nullptr, r=nullptr, i=1
      {Operation::CreateRefFromNullptr, Operation::ResetRefFromNullptr,
       Operation::ValidateRefEmpty, Operation::ValidateRefEmpty},
      // Testcase 35, i=nullptr, r=nullptr, i=2
      {Operation::CreateRefFromNullptr, Operation::ResetRefFromNullptr,
       Operation::ValidateRefEmpty, Operation::ValidateRefEmpty},
      // Testcase 36, i=nullptr, r=nullptr, i=3
      {Operation::CreateRefFromNullptr, Operation::ResetRefFromNullptr,
       Operation::ValidateRefEmpty, Operation::ValidateRefEmpty},
      // Testcase 37, i=nullptr, r=empty_id, i=0
      {Operation::CreateRefFromNullptr, Operation::ResetRefFromEmptyId,
       Operation::ValidateRefEmpty, Operation::ValidateRefEmpty},
      // Testcase 38, i=nullptr, r=empty_id, i=1
      {Operation::CreateRefFromNullptr, Operation::ResetRefFromEmptyId,
       Operation::ValidateRefEmpty, Operation::ValidateRefEmpty},
      // Testcase 39, i=nullptr, r=empty_id, i=2
      {Operation::CreateRefFromNullptr, Operation::ResetRefFromEmptyId,
       Operation::ValidateRefEmpty, Operation::ValidateRefEmpty},
      // Testcase 40, i=nullptr, r=empty_id, i=3
      {Operation::CreateRefFromNullptr, Operation::ResetRefFromEmptyId,
       Operation::ValidateRefEmpty, Operation::ValidateRefEmpty},
      // Testcase 41, i=nullptr, r=id, i=0
      {Operation::CreateRefFromNullptr, Operation::CreateEntity2,
       Operation::ResetRefFromId, Operation::RefGetPtr,
       Operation::ValidateRefToEntity2, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity2, Operation::ValidateRefEmpty},
      // Testcase 42, i=nullptr, r=id, i=1
      {Operation::CreateRefFromNullptr, Operation::CreateEntity2,
       Operation::ResetRefFromId, Operation::RefFillPtr,
       Operation::ValidateRefToEntity2, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity2, Operation::ValidateRefEmpty},
      // Testcase 43, i=nullptr, r=id, i=2
      {Operation::CreateRefFromNullptr, Operation::CreateEntity2,
       Operation::ResetRefFromId, Operation::ValidateRefToEntity2IdOnly,
       Operation::RemoveEntity2, Operation::ValidateRefToEntity2IdOnly},
      // Testcase 44, i=nullptr, r=id, i=3
      {Operation::CreateRefFromNullptr, Operation::CreateEntity2,
       Operation::ResetRefFromId, Operation::ValidateRefToEntity2IdOnly,
       Operation::RemoveEntity2, Operation::ValidateRefToEntity2IdOnly},
      // Testcase 45, i=nullptr, r=reg_id, i=0
      {Operation::CreateRefFromNullptr, Operation::CreateEntity2,
       Operation::MgrProcessChanges, Operation::ResetRefFromId,
       Operation::RefGetPtr, Operation::ValidateRefToEntity2,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2, Operation::MgrProcessChanges,
       Operation::ValidateRefEmpty},
      // Testcase 46, i=nullptr, r=reg_id, i=1
      {Operation::CreateRefFromNullptr, Operation::CreateEntity2,
       Operation::MgrProcessChanges, Operation::ResetRefFromId,
       Operation::RefFillPtr, Operation::ValidateRefToEntity2,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2, Operation::MgrProcessChanges,
       Operation::ValidateRefEmpty},
      // Testcase 47, i=nullptr, r=reg_id, i=2
      {Operation::CreateRefFromNullptr, Operation::CreateEntity2,
       Operation::MgrProcessChanges, Operation::ResetRefFromId,
       Operation::ValidateRefToEntity2IdOnly, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2IdOnly},
      // Testcase 48, i=nullptr, r=reg_id, i=3
      {Operation::CreateRefFromNullptr, Operation::CreateEntity2,
       Operation::MgrProcessChanges, Operation::ResetRefFromId,
       Operation::ValidateRefToEntity2IdOnly, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2IdOnly},
      // Testcase 49, i=nullptr, r=entity, i=0
      {Operation::CreateRefFromNullptr, Operation::CreateEntity2,
       Operation::ResetRefFromEntityPtr, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity2, Operation::ValidateRefEmpty},
      // Testcase 50, i=nullptr, r=entity, i=1
      {Operation::CreateRefFromNullptr, Operation::CreateEntity2,
       Operation::ResetRefFromEntityPtr, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity2, Operation::ValidateRefEmpty},
      // Testcase 51, i=nullptr, r=entity, i=2
      {Operation::CreateRefFromNullptr, Operation::CreateEntity2,
       Operation::ResetRefFromEntityPtr, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity2, Operation::ValidateRefEmpty},
      // Testcase 52, i=nullptr, r=entity, i=3
      {Operation::CreateRefFromNullptr, Operation::CreateEntity2,
       Operation::ResetRefFromEntityPtr, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity2, Operation::ValidateRefEmpty},
      // Testcase 53, i=nullptr, r=reg_entity, i=0
      {Operation::CreateRefFromNullptr, Operation::CreateEntity2,
       Operation::MgrProcessChanges, Operation::ResetRefFromEntityPtr,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2, Operation::MgrProcessChanges,
       Operation::ValidateRefEmpty},
      // Testcase 54, i=nullptr, r=reg_entity, i=1
      {Operation::CreateRefFromNullptr, Operation::CreateEntity2,
       Operation::MgrProcessChanges, Operation::ResetRefFromEntityPtr,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2, Operation::MgrProcessChanges,
       Operation::ValidateRefEmpty},
      // Testcase 55, i=nullptr, r=reg_entity, i=2
      {Operation::CreateRefFromNullptr, Operation::CreateEntity2,
       Operation::MgrProcessChanges, Operation::ResetRefFromEntityPtr,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2, Operation::MgrProcessChanges,
       Operation::ValidateRefEmpty},
      // Testcase 56, i=nullptr, r=reg_entity, i=3
      {Operation::CreateRefFromNullptr, Operation::CreateEntity2,
       Operation::MgrProcessChanges, Operation::ResetRefFromEntityPtr,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2, Operation::MgrProcessChanges,
       Operation::ValidateRefEmpty},
      // Testcase 57, i=empty_id, r=empty, i=0
      {Operation::CreateRefFromEmptyId, Operation::ResetRefFromEmpty,
       Operation::ValidateRefEmpty, Operation::ValidateRefEmpty},
      // Testcase 58, i=empty_id, r=empty, i=1
      {Operation::CreateRefFromEmptyId, Operation::ResetRefFromEmpty,
       Operation::ValidateRefEmpty, Operation::ValidateRefEmpty},
      // Testcase 59, i=empty_id, r=empty, i=2
      {Operation::CreateRefFromEmptyId, Operation::ResetRefFromEmpty,
       Operation::ValidateRefEmpty, Operation::ValidateRefEmpty},
      // Testcase 60, i=empty_id, r=empty, i=3
      {Operation::CreateRefFromEmptyId, Operation::ResetRefFromEmpty,
       Operation::ValidateRefEmpty, Operation::ValidateRefEmpty},
      // Testcase 61, i=empty_id, r=nullptr, i=0
      {Operation::CreateRefFromEmptyId, Operation::ResetRefFromNullptr,
       Operation::ValidateRefEmpty, Operation::ValidateRefEmpty},
      // Testcase 62, i=empty_id, r=nullptr, i=1
      {Operation::CreateRefFromEmptyId, Operation::ResetRefFromNullptr,
       Operation::ValidateRefEmpty, Operation::ValidateRefEmpty},
      // Testcase 63, i=empty_id, r=nullptr, i=2
      {Operation::CreateRefFromEmptyId, Operation::ResetRefFromNullptr,
       Operation::ValidateRefEmpty, Operation::ValidateRefEmpty},
      // Testcase 64, i=empty_id, r=nullptr, i=3
      {Operation::CreateRefFromEmptyId, Operation::ResetRefFromNullptr,
       Operation::ValidateRefEmpty, Operation::ValidateRefEmpty},
      // Testcase 65, i=empty_id, r=empty_id, i=0
      {Operation::CreateRefFromEmptyId, Operation::ResetRefFromEmptyId,
       Operation::ValidateRefEmpty, Operation::ValidateRefEmpty},
      // Testcase 66, i=empty_id, r=empty_id, i=1
      {Operation::CreateRefFromEmptyId, Operation::ResetRefFromEmptyId,
       Operation::ValidateRefEmpty, Operation::ValidateRefEmpty},
      // Testcase 67, i=empty_id, r=empty_id, i=2
      {Operation::CreateRefFromEmptyId, Operation::ResetRefFromEmptyId,
       Operation::ValidateRefEmpty, Operation::ValidateRefEmpty},
      // Testcase 68, i=empty_id, r=empty_id, i=3
      {Operation::CreateRefFromEmptyId, Operation::ResetRefFromEmptyId,
       Operation::ValidateRefEmpty, Operation::ValidateRefEmpty},
      // Testcase 69, i=empty_id, r=id, i=0
      {Operation::CreateRefFromEmptyId, Operation::CreateEntity2,
       Operation::ResetRefFromId, Operation::RefGetPtr,
       Operation::ValidateRefToEntity2, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity2, Operation::ValidateRefEmpty},
      // Testcase 70, i=empty_id, r=id, i=1
      {Operation::CreateRefFromEmptyId, Operation::CreateEntity2,
       Operation::ResetRefFromId, Operation::RefFillPtr,
       Operation::ValidateRefToEntity2, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity2, Operation::ValidateRefEmpty},
      // Testcase 71, i=empty_id, r=id, i=2
      {Operation::CreateRefFromEmptyId, Operation::CreateEntity2,
       Operation::ResetRefFromId, Operation::ValidateRefToEntity2IdOnly,
       Operation::RemoveEntity2, Operation::ValidateRefToEntity2IdOnly},
      // Testcase 72, i=empty_id, r=id, i=3
      {Operation::CreateRefFromEmptyId, Operation::CreateEntity2,
       Operation::ResetRefFromId, Operation::ValidateRefToEntity2IdOnly,
       Operation::RemoveEntity2, Operation::ValidateRefToEntity2IdOnly},
      // Testcase 73, i=empty_id, r=reg_id, i=0
      {Operation::CreateRefFromEmptyId, Operation::CreateEntity2,
       Operation::MgrProcessChanges, Operation::ResetRefFromId,
       Operation::RefGetPtr, Operation::ValidateRefToEntity2,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2, Operation::MgrProcessChanges,
       Operation::ValidateRefEmpty},
      // Testcase 74, i=empty_id, r=reg_id, i=1
      {Operation::CreateRefFromEmptyId, Operation::CreateEntity2,
       Operation::MgrProcessChanges, Operation::ResetRefFromId,
       Operation::RefFillPtr, Operation::ValidateRefToEntity2,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2, Operation::MgrProcessChanges,
       Operation::ValidateRefEmpty},
      // Testcase 75, i=empty_id, r=reg_id, i=2
      {Operation::CreateRefFromEmptyId, Operation::CreateEntity2,
       Operation::MgrProcessChanges, Operation::ResetRefFromId,
       Operation::ValidateRefToEntity2IdOnly, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2IdOnly},
      // Testcase 76, i=empty_id, r=reg_id, i=3
      {Operation::CreateRefFromEmptyId, Operation::CreateEntity2,
       Operation::MgrProcessChanges, Operation::ResetRefFromId,
       Operation::ValidateRefToEntity2IdOnly, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2IdOnly},
      // Testcase 77, i=empty_id, r=entity, i=0
      {Operation::CreateRefFromEmptyId, Operation::CreateEntity2,
       Operation::ResetRefFromEntityPtr, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity2, Operation::ValidateRefEmpty},
      // Testcase 78, i=empty_id, r=entity, i=1
      {Operation::CreateRefFromEmptyId, Operation::CreateEntity2,
       Operation::ResetRefFromEntityPtr, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity2, Operation::ValidateRefEmpty},
      // Testcase 79, i=empty_id, r=entity, i=2
      {Operation::CreateRefFromEmptyId, Operation::CreateEntity2,
       Operation::ResetRefFromEntityPtr, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity2, Operation::ValidateRefEmpty},
      // Testcase 80, i=empty_id, r=entity, i=3
      {Operation::CreateRefFromEmptyId, Operation::CreateEntity2,
       Operation::ResetRefFromEntityPtr, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity2, Operation::ValidateRefEmpty},
      // Testcase 81, i=empty_id, r=reg_entity, i=0
      {Operation::CreateRefFromEmptyId, Operation::CreateEntity2,
       Operation::MgrProcessChanges, Operation::ResetRefFromEntityPtr,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2, Operation::MgrProcessChanges,
       Operation::ValidateRefEmpty},
      // Testcase 82, i=empty_id, r=reg_entity, i=1
      {Operation::CreateRefFromEmptyId, Operation::CreateEntity2,
       Operation::MgrProcessChanges, Operation::ResetRefFromEntityPtr,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2, Operation::MgrProcessChanges,
       Operation::ValidateRefEmpty},
      // Testcase 83, i=empty_id, r=reg_entity, i=2
      {Operation::CreateRefFromEmptyId, Operation::CreateEntity2,
       Operation::MgrProcessChanges, Operation::ResetRefFromEntityPtr,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2, Operation::MgrProcessChanges,
       Operation::ValidateRefEmpty},
      // Testcase 84, i=empty_id, r=reg_entity, i=3
      {Operation::CreateRefFromEmptyId, Operation::CreateEntity2,
       Operation::MgrProcessChanges, Operation::ResetRefFromEntityPtr,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2, Operation::MgrProcessChanges,
       Operation::ValidateRefEmpty},
      // Testcase 85, i=id, r=empty, i=0
      {Operation::CreateEntity1, Operation::CreateRefFromId,
       Operation::ResetRefFromEmpty, Operation::ValidateRefEmpty,
       Operation::RemoveEntity1, Operation::ValidateRefEmpty},
      // Testcase 86, i=id, r=empty, i=1
      {Operation::CreateEntity1, Operation::CreateRefFromId,
       Operation::ResetRefFromEmpty, Operation::ValidateRefEmpty,
       Operation::RemoveEntity1, Operation::ValidateRefEmpty},
      // Testcase 87, i=id, r=empty, i=2
      {Operation::CreateEntity1, Operation::CreateRefFromId,
       Operation::ResetRefFromEmpty, Operation::ValidateRefEmpty,
       Operation::RemoveEntity1, Operation::ValidateRefEmpty},
      // Testcase 88, i=id, r=empty, i=3
      {Operation::CreateEntity1, Operation::CreateRefFromId,
       Operation::ResetRefFromEmpty, Operation::ValidateRefEmpty,
       Operation::RemoveEntity1, Operation::ValidateRefEmpty},
      // Testcase 89, i=id, r=nullptr, i=0
      {Operation::CreateEntity1, Operation::CreateRefFromId,
       Operation::ResetRefFromNullptr, Operation::ValidateRefEmpty,
       Operation::RemoveEntity1, Operation::ValidateRefEmpty},
      // Testcase 90, i=id, r=nullptr, i=1
      {Operation::CreateEntity1, Operation::CreateRefFromId,
       Operation::ResetRefFromNullptr, Operation::ValidateRefEmpty,
       Operation::RemoveEntity1, Operation::ValidateRefEmpty},
      // Testcase 91, i=id, r=nullptr, i=2
      {Operation::CreateEntity1, Operation::CreateRefFromId,
       Operation::ResetRefFromNullptr, Operation::ValidateRefEmpty,
       Operation::RemoveEntity1, Operation::ValidateRefEmpty},
      // Testcase 92, i=id, r=nullptr, i=3
      {Operation::CreateEntity1, Operation::CreateRefFromId,
       Operation::ResetRefFromNullptr, Operation::ValidateRefEmpty,
       Operation::RemoveEntity1, Operation::ValidateRefEmpty},
      // Testcase 93, i=id, r=empty_id, i=0
      {Operation::CreateEntity1, Operation::CreateRefFromId,
       Operation::ResetRefFromEmptyId, Operation::ValidateRefEmpty,
       Operation::RemoveEntity1, Operation::ValidateRefEmpty},
      // Testcase 94, i=id, r=empty_id, i=1
      {Operation::CreateEntity1, Operation::CreateRefFromId,
       Operation::ResetRefFromEmptyId, Operation::ValidateRefEmpty,
       Operation::RemoveEntity1, Operation::ValidateRefEmpty},
      // Testcase 95, i=id, r=empty_id, i=2
      {Operation::CreateEntity1, Operation::CreateRefFromId,
       Operation::ResetRefFromEmptyId, Operation::ValidateRefEmpty,
       Operation::RemoveEntity1, Operation::ValidateRefEmpty},
      // Testcase 96, i=id, r=empty_id, i=3
      {Operation::CreateEntity1, Operation::CreateRefFromId,
       Operation::ResetRefFromEmptyId, Operation::ValidateRefEmpty,
       Operation::RemoveEntity1, Operation::ValidateRefEmpty},
      // Testcase 97, i=id, r=id, i=0
      {Operation::CreateEntity1, Operation::CreateRefFromId,
       Operation::CreateEntity2, Operation::ResetRefFromId,
       Operation::RefGetPtr, Operation::ValidateRefToEntity2,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity1,
       Operation::RemoveEntity2, Operation::ValidateRefEmpty},
      // Testcase 98, i=id, r=id, i=1
      {Operation::CreateEntity1, Operation::CreateRefFromId,
       Operation::CreateEntity2, Operation::ResetRefFromId,
       Operation::RefFillPtr, Operation::ValidateRefToEntity2,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity1,
       Operation::RemoveEntity2, Operation::ValidateRefEmpty},
      // Testcase 99, i=id, r=id, i=2
      {Operation::CreateEntity1, Operation::CreateRefFromId,
       Operation::CreateEntity2, Operation::ResetRefFromId,
       Operation::ValidateRefToEntity2IdOnly, Operation::RemoveEntity1,
       Operation::RemoveEntity2, Operation::ValidateRefToEntity2IdOnly},
      // Testcase 100, i=id, r=id, i=3
      {Operation::CreateEntity1, Operation::CreateRefFromId,
       Operation::CreateEntity2, Operation::ResetRefFromId,
       Operation::ValidateRefToEntity2IdOnly, Operation::RemoveEntity1,
       Operation::RemoveEntity2, Operation::ValidateRefToEntity2IdOnly},
      // Testcase 101, i=id, r=reg_id, i=0
      {Operation::CreateEntity1, Operation::CreateRefFromId,
       Operation::CreateEntity2, Operation::MgrProcessChanges,
       Operation::ResetRefFromId, Operation::RefGetPtr,
       Operation::ValidateRefToEntity2, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity1, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2, Operation::MgrProcessChanges,
       Operation::ValidateRefEmpty},
      // Testcase 102, i=id, r=reg_id, i=1
      {Operation::CreateEntity1, Operation::CreateRefFromId,
       Operation::CreateEntity2, Operation::MgrProcessChanges,
       Operation::ResetRefFromId, Operation::RefFillPtr,
       Operation::ValidateRefToEntity2, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity1, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2, Operation::MgrProcessChanges,
       Operation::ValidateRefEmpty},
      // Testcase 103, i=id, r=reg_id, i=2
      {Operation::CreateEntity1, Operation::CreateRefFromId,
       Operation::CreateEntity2, Operation::MgrProcessChanges,
       Operation::ResetRefFromId, Operation::ValidateRefToEntity2IdOnly,
       Operation::RemoveEntity1, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2IdOnly},
      // Testcase 104, i=id, r=reg_id, i=3
      {Operation::CreateEntity1, Operation::CreateRefFromId,
       Operation::CreateEntity2, Operation::MgrProcessChanges,
       Operation::ResetRefFromId, Operation::ValidateRefToEntity2IdOnly,
       Operation::RemoveEntity1, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2IdOnly},
      // Testcase 105, i=id, r=entity, i=0
      {Operation::CreateEntity1, Operation::CreateRefFromId,
       Operation::CreateEntity2, Operation::ResetRefFromEntityPtr,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity1,
       Operation::RemoveEntity2, Operation::ValidateRefEmpty},
      // Testcase 106, i=id, r=entity, i=1
      {Operation::CreateEntity1, Operation::CreateRefFromId,
       Operation::CreateEntity2, Operation::ResetRefFromEntityPtr,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity1,
       Operation::RemoveEntity2, Operation::ValidateRefEmpty},
      // Testcase 107, i=id, r=entity, i=2
      {Operation::CreateEntity1, Operation::CreateRefFromId,
       Operation::CreateEntity2, Operation::ResetRefFromEntityPtr,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity1,
       Operation::RemoveEntity2, Operation::ValidateRefEmpty},
      // Testcase 108, i=id, r=entity, i=3
      {Operation::CreateEntity1, Operation::CreateRefFromId,
       Operation::CreateEntity2, Operation::ResetRefFromEntityPtr,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity1,
       Operation::RemoveEntity2, Operation::ValidateRefEmpty},
      // Testcase 109, i=id, r=reg_entity, i=0
      {Operation::CreateEntity1, Operation::CreateRefFromId,
       Operation::CreateEntity2, Operation::MgrProcessChanges,
       Operation::ResetRefFromEntityPtr, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity1, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2, Operation::MgrProcessChanges,
       Operation::ValidateRefEmpty},
      // Testcase 110, i=id, r=reg_entity, i=1
      {Operation::CreateEntity1, Operation::CreateRefFromId,
       Operation::CreateEntity2, Operation::MgrProcessChanges,
       Operation::ResetRefFromEntityPtr, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity1, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2, Operation::MgrProcessChanges,
       Operation::ValidateRefEmpty},
      // Testcase 111, i=id, r=reg_entity, i=2
      {Operation::CreateEntity1, Operation::CreateRefFromId,
       Operation::CreateEntity2, Operation::MgrProcessChanges,
       Operation::ResetRefFromEntityPtr, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity1, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2, Operation::MgrProcessChanges,
       Operation::ValidateRefEmpty},
      // Testcase 112, i=id, r=reg_entity, i=3
      {Operation::CreateEntity1, Operation::CreateRefFromId,
       Operation::CreateEntity2, Operation::MgrProcessChanges,
       Operation::ResetRefFromEntityPtr, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity1, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2, Operation::MgrProcessChanges,
       Operation::ValidateRefEmpty},
      // Testcase 113, i=reg_id, r=empty, i=0
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromId, Operation::ResetRefFromEmpty,
       Operation::ValidateRefEmpty, Operation::RemoveEntity1,
       Operation::ValidateRefEmpty},
      // Testcase 114, i=reg_id, r=empty, i=1
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromId, Operation::ResetRefFromEmpty,
       Operation::ValidateRefEmpty, Operation::RemoveEntity1,
       Operation::ValidateRefEmpty},
      // Testcase 115, i=reg_id, r=empty, i=2
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromId, Operation::ResetRefFromEmpty,
       Operation::ValidateRefEmpty, Operation::RemoveEntity1,
       Operation::ValidateRefEmpty},
      // Testcase 116, i=reg_id, r=empty, i=3
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromId, Operation::ResetRefFromEmpty,
       Operation::ValidateRefEmpty, Operation::RemoveEntity1,
       Operation::ValidateRefEmpty},
      // Testcase 117, i=reg_id, r=nullptr, i=0
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromId, Operation::ResetRefFromNullptr,
       Operation::ValidateRefEmpty, Operation::RemoveEntity1,
       Operation::ValidateRefEmpty},
      // Testcase 118, i=reg_id, r=nullptr, i=1
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromId, Operation::ResetRefFromNullptr,
       Operation::ValidateRefEmpty, Operation::RemoveEntity1,
       Operation::ValidateRefEmpty},
      // Testcase 119, i=reg_id, r=nullptr, i=2
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromId, Operation::ResetRefFromNullptr,
       Operation::ValidateRefEmpty, Operation::RemoveEntity1,
       Operation::ValidateRefEmpty},
      // Testcase 120, i=reg_id, r=nullptr, i=3
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromId, Operation::ResetRefFromNullptr,
       Operation::ValidateRefEmpty, Operation::RemoveEntity1,
       Operation::ValidateRefEmpty},
      // Testcase 121, i=reg_id, r=empty_id, i=0
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromId, Operation::ResetRefFromEmptyId,
       Operation::ValidateRefEmpty, Operation::RemoveEntity1,
       Operation::ValidateRefEmpty},
      // Testcase 122, i=reg_id, r=empty_id, i=1
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromId, Operation::ResetRefFromEmptyId,
       Operation::ValidateRefEmpty, Operation::RemoveEntity1,
       Operation::ValidateRefEmpty},
      // Testcase 123, i=reg_id, r=empty_id, i=2
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromId, Operation::ResetRefFromEmptyId,
       Operation::ValidateRefEmpty, Operation::RemoveEntity1,
       Operation::ValidateRefEmpty},
      // Testcase 124, i=reg_id, r=empty_id, i=3
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromId, Operation::ResetRefFromEmptyId,
       Operation::ValidateRefEmpty, Operation::RemoveEntity1,
       Operation::ValidateRefEmpty},
      // Testcase 125, i=reg_id, r=id, i=0
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromId, Operation::CreateEntity2,
       Operation::ResetRefFromId, Operation::RefGetPtr,
       Operation::ValidateRefToEntity2, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity1, Operation::RemoveEntity2,
       Operation::ValidateRefEmpty},
      // Testcase 126, i=reg_id, r=id, i=1
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromId, Operation::CreateEntity2,
       Operation::ResetRefFromId, Operation::RefFillPtr,
       Operation::ValidateRefToEntity2, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity1, Operation::RemoveEntity2,
       Operation::ValidateRefEmpty},
      // Testcase 127, i=reg_id, r=id, i=2
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromId, Operation::CreateEntity2,
       Operation::ResetRefFromId, Operation::ValidateRefToEntity2IdOnly,
       Operation::RemoveEntity1, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2IdOnly},
      // Testcase 128, i=reg_id, r=id, i=3
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromId, Operation::CreateEntity2,
       Operation::ResetRefFromId, Operation::ValidateRefToEntity2IdOnly,
       Operation::RemoveEntity1, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2IdOnly},
      // Testcase 129, i=reg_id, r=reg_id, i=0
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromId, Operation::CreateEntity2,
       Operation::MgrProcessChanges, Operation::ResetRefFromId,
       Operation::RefGetPtr, Operation::ValidateRefToEntity2,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity1,
       Operation::RemoveEntity2, Operation::ValidateRefToEntity2,
       Operation::MgrProcessChanges, Operation::ValidateRefEmpty},
      // Testcase 130, i=reg_id, r=reg_id, i=1
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromId, Operation::CreateEntity2,
       Operation::MgrProcessChanges, Operation::ResetRefFromId,
       Operation::RefFillPtr, Operation::ValidateRefToEntity2,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity1,
       Operation::RemoveEntity2, Operation::ValidateRefToEntity2,
       Operation::MgrProcessChanges, Operation::ValidateRefEmpty},
      // Testcase 131, i=reg_id, r=reg_id, i=2
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromId, Operation::CreateEntity2,
       Operation::MgrProcessChanges, Operation::ResetRefFromId,
       Operation::ValidateRefToEntity2IdOnly, Operation::RemoveEntity1,
       Operation::RemoveEntity2, Operation::ValidateRefToEntity2IdOnly},
      // Testcase 132, i=reg_id, r=reg_id, i=3
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromId, Operation::CreateEntity2,
       Operation::MgrProcessChanges, Operation::ResetRefFromId,
       Operation::ValidateRefToEntity2IdOnly, Operation::RemoveEntity1,
       Operation::RemoveEntity2, Operation::ValidateRefToEntity2IdOnly},
      // Testcase 133, i=reg_id, r=entity, i=0
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromId, Operation::CreateEntity2,
       Operation::ResetRefFromEntityPtr, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity1, Operation::RemoveEntity2,
       Operation::ValidateRefEmpty},
      // Testcase 134, i=reg_id, r=entity, i=1
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromId, Operation::CreateEntity2,
       Operation::ResetRefFromEntityPtr, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity1, Operation::RemoveEntity2,
       Operation::ValidateRefEmpty},
      // Testcase 135, i=reg_id, r=entity, i=2
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromId, Operation::CreateEntity2,
       Operation::ResetRefFromEntityPtr, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity1, Operation::RemoveEntity2,
       Operation::ValidateRefEmpty},
      // Testcase 136, i=reg_id, r=entity, i=3
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromId, Operation::CreateEntity2,
       Operation::ResetRefFromEntityPtr, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity1, Operation::RemoveEntity2,
       Operation::ValidateRefEmpty},
      // Testcase 137, i=reg_id, r=reg_entity, i=0
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromId, Operation::CreateEntity2,
       Operation::MgrProcessChanges, Operation::ResetRefFromEntityPtr,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity1,
       Operation::RemoveEntity2, Operation::ValidateRefToEntity2,
       Operation::MgrProcessChanges, Operation::ValidateRefEmpty},
      // Testcase 138, i=reg_id, r=reg_entity, i=1
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromId, Operation::CreateEntity2,
       Operation::MgrProcessChanges, Operation::ResetRefFromEntityPtr,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity1,
       Operation::RemoveEntity2, Operation::ValidateRefToEntity2,
       Operation::MgrProcessChanges, Operation::ValidateRefEmpty},
      // Testcase 139, i=reg_id, r=reg_entity, i=2
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromId, Operation::CreateEntity2,
       Operation::MgrProcessChanges, Operation::ResetRefFromEntityPtr,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity1,
       Operation::RemoveEntity2, Operation::ValidateRefToEntity2,
       Operation::MgrProcessChanges, Operation::ValidateRefEmpty},
      // Testcase 140, i=reg_id, r=reg_entity, i=3
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromId, Operation::CreateEntity2,
       Operation::MgrProcessChanges, Operation::ResetRefFromEntityPtr,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity1,
       Operation::RemoveEntity2, Operation::ValidateRefToEntity2,
       Operation::MgrProcessChanges, Operation::ValidateRefEmpty},
      // Testcase 141, i=entity, r=empty, i=0
      {Operation::CreateEntity1, Operation::CreateRefFromEntityPtr,
       Operation::ResetRefFromEmpty, Operation::ValidateRefEmpty,
       Operation::RemoveEntity1, Operation::ValidateRefEmpty},
      // Testcase 142, i=entity, r=empty, i=1
      {Operation::CreateEntity1, Operation::CreateRefFromEntityPtr,
       Operation::ResetRefFromEmpty, Operation::ValidateRefEmpty,
       Operation::RemoveEntity1, Operation::ValidateRefEmpty},
      // Testcase 143, i=entity, r=empty, i=2
      {Operation::CreateEntity1, Operation::CreateRefFromEntityPtr,
       Operation::ResetRefFromEmpty, Operation::ValidateRefEmpty,
       Operation::RemoveEntity1, Operation::ValidateRefEmpty},
      // Testcase 144, i=entity, r=empty, i=3
      {Operation::CreateEntity1, Operation::CreateRefFromEntityPtr,
       Operation::ResetRefFromEmpty, Operation::ValidateRefEmpty,
       Operation::RemoveEntity1, Operation::ValidateRefEmpty},
      // Testcase 145, i=entity, r=nullptr, i=0
      {Operation::CreateEntity1, Operation::CreateRefFromEntityPtr,
       Operation::ResetRefFromNullptr, Operation::ValidateRefEmpty,
       Operation::RemoveEntity1, Operation::ValidateRefEmpty},
      // Testcase 146, i=entity, r=nullptr, i=1
      {Operation::CreateEntity1, Operation::CreateRefFromEntityPtr,
       Operation::ResetRefFromNullptr, Operation::ValidateRefEmpty,
       Operation::RemoveEntity1, Operation::ValidateRefEmpty},
      // Testcase 147, i=entity, r=nullptr, i=2
      {Operation::CreateEntity1, Operation::CreateRefFromEntityPtr,
       Operation::ResetRefFromNullptr, Operation::ValidateRefEmpty,
       Operation::RemoveEntity1, Operation::ValidateRefEmpty},
      // Testcase 148, i=entity, r=nullptr, i=3
      {Operation::CreateEntity1, Operation::CreateRefFromEntityPtr,
       Operation::ResetRefFromNullptr, Operation::ValidateRefEmpty,
       Operation::RemoveEntity1, Operation::ValidateRefEmpty},
      // Testcase 149, i=entity, r=empty_id, i=0
      {Operation::CreateEntity1, Operation::CreateRefFromEntityPtr,
       Operation::ResetRefFromEmptyId, Operation::ValidateRefEmpty,
       Operation::RemoveEntity1, Operation::ValidateRefEmpty},
      // Testcase 150, i=entity, r=empty_id, i=1
      {Operation::CreateEntity1, Operation::CreateRefFromEntityPtr,
       Operation::ResetRefFromEmptyId, Operation::ValidateRefEmpty,
       Operation::RemoveEntity1, Operation::ValidateRefEmpty},
      // Testcase 151, i=entity, r=empty_id, i=2
      {Operation::CreateEntity1, Operation::CreateRefFromEntityPtr,
       Operation::ResetRefFromEmptyId, Operation::ValidateRefEmpty,
       Operation::RemoveEntity1, Operation::ValidateRefEmpty},
      // Testcase 152, i=entity, r=empty_id, i=3
      {Operation::CreateEntity1, Operation::CreateRefFromEntityPtr,
       Operation::ResetRefFromEmptyId, Operation::ValidateRefEmpty,
       Operation::RemoveEntity1, Operation::ValidateRefEmpty},
      // Testcase 153, i=entity, r=id, i=0
      {Operation::CreateEntity1, Operation::CreateRefFromEntityPtr,
       Operation::CreateEntity2, Operation::ResetRefFromId,
       Operation::RefGetPtr, Operation::ValidateRefToEntity2,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity1,
       Operation::RemoveEntity2, Operation::ValidateRefEmpty},
      // Testcase 154, i=entity, r=id, i=1
      {Operation::CreateEntity1, Operation::CreateRefFromEntityPtr,
       Operation::CreateEntity2, Operation::ResetRefFromId,
       Operation::RefFillPtr, Operation::ValidateRefToEntity2,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity1,
       Operation::RemoveEntity2, Operation::ValidateRefEmpty},
      // Testcase 155, i=entity, r=id, i=2
      {Operation::CreateEntity1, Operation::CreateRefFromEntityPtr,
       Operation::CreateEntity2, Operation::ResetRefFromId,
       Operation::ValidateRefToEntity2IdOnly, Operation::RemoveEntity1,
       Operation::RemoveEntity2, Operation::ValidateRefToEntity2IdOnly},
      // Testcase 156, i=entity, r=id, i=3
      {Operation::CreateEntity1, Operation::CreateRefFromEntityPtr,
       Operation::CreateEntity2, Operation::ResetRefFromId,
       Operation::ValidateRefToEntity2IdOnly, Operation::RemoveEntity1,
       Operation::RemoveEntity2, Operation::ValidateRefToEntity2IdOnly},
      // Testcase 157, i=entity, r=reg_id, i=0
      {Operation::CreateEntity1, Operation::CreateRefFromEntityPtr,
       Operation::CreateEntity2, Operation::MgrProcessChanges,
       Operation::ResetRefFromId, Operation::RefGetPtr,
       Operation::ValidateRefToEntity2, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity1, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2, Operation::MgrProcessChanges,
       Operation::ValidateRefEmpty},
      // Testcase 158, i=entity, r=reg_id, i=1
      {Operation::CreateEntity1, Operation::CreateRefFromEntityPtr,
       Operation::CreateEntity2, Operation::MgrProcessChanges,
       Operation::ResetRefFromId, Operation::RefFillPtr,
       Operation::ValidateRefToEntity2, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity1, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2, Operation::MgrProcessChanges,
       Operation::ValidateRefEmpty},
      // Testcase 159, i=entity, r=reg_id, i=2
      {Operation::CreateEntity1, Operation::CreateRefFromEntityPtr,
       Operation::CreateEntity2, Operation::MgrProcessChanges,
       Operation::ResetRefFromId, Operation::ValidateRefToEntity2IdOnly,
       Operation::RemoveEntity1, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2IdOnly},
      // Testcase 160, i=entity, r=reg_id, i=3
      {Operation::CreateEntity1, Operation::CreateRefFromEntityPtr,
       Operation::CreateEntity2, Operation::MgrProcessChanges,
       Operation::ResetRefFromId, Operation::ValidateRefToEntity2IdOnly,
       Operation::RemoveEntity1, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2IdOnly},
      // Testcase 161, i=entity, r=entity, i=0
      {Operation::CreateEntity1, Operation::CreateRefFromEntityPtr,
       Operation::CreateEntity2, Operation::ResetRefFromEntityPtr,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity1,
       Operation::RemoveEntity2, Operation::ValidateRefEmpty},
      // Testcase 162, i=entity, r=entity, i=1
      {Operation::CreateEntity1, Operation::CreateRefFromEntityPtr,
       Operation::CreateEntity2, Operation::ResetRefFromEntityPtr,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity1,
       Operation::RemoveEntity2, Operation::ValidateRefEmpty},
      // Testcase 163, i=entity, r=entity, i=2
      {Operation::CreateEntity1, Operation::CreateRefFromEntityPtr,
       Operation::CreateEntity2, Operation::ResetRefFromEntityPtr,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity1,
       Operation::RemoveEntity2, Operation::ValidateRefEmpty},
      // Testcase 164, i=entity, r=entity, i=3
      {Operation::CreateEntity1, Operation::CreateRefFromEntityPtr,
       Operation::CreateEntity2, Operation::ResetRefFromEntityPtr,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity1,
       Operation::RemoveEntity2, Operation::ValidateRefEmpty},
      // Testcase 165, i=entity, r=reg_entity, i=0
      {Operation::CreateEntity1, Operation::CreateRefFromEntityPtr,
       Operation::CreateEntity2, Operation::MgrProcessChanges,
       Operation::ResetRefFromEntityPtr, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity1, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2, Operation::MgrProcessChanges,
       Operation::ValidateRefEmpty},
      // Testcase 166, i=entity, r=reg_entity, i=1
      {Operation::CreateEntity1, Operation::CreateRefFromEntityPtr,
       Operation::CreateEntity2, Operation::MgrProcessChanges,
       Operation::ResetRefFromEntityPtr, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity1, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2, Operation::MgrProcessChanges,
       Operation::ValidateRefEmpty},
      // Testcase 167, i=entity, r=reg_entity, i=2
      {Operation::CreateEntity1, Operation::CreateRefFromEntityPtr,
       Operation::CreateEntity2, Operation::MgrProcessChanges,
       Operation::ResetRefFromEntityPtr, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity1, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2, Operation::MgrProcessChanges,
       Operation::ValidateRefEmpty},
      // Testcase 168, i=entity, r=reg_entity, i=3
      {Operation::CreateEntity1, Operation::CreateRefFromEntityPtr,
       Operation::CreateEntity2, Operation::MgrProcessChanges,
       Operation::ResetRefFromEntityPtr, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity1, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2, Operation::MgrProcessChanges,
       Operation::ValidateRefEmpty},
      // Testcase 169, i=reg_entity, r=empty, i=0
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromEntityPtr, Operation::ResetRefFromEmpty,
       Operation::ValidateRefEmpty, Operation::RemoveEntity1,
       Operation::ValidateRefEmpty},
      // Testcase 170, i=reg_entity, r=empty, i=1
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromEntityPtr, Operation::ResetRefFromEmpty,
       Operation::ValidateRefEmpty, Operation::RemoveEntity1,
       Operation::ValidateRefEmpty},
      // Testcase 171, i=reg_entity, r=empty, i=2
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromEntityPtr, Operation::ResetRefFromEmpty,
       Operation::ValidateRefEmpty, Operation::RemoveEntity1,
       Operation::ValidateRefEmpty},
      // Testcase 172, i=reg_entity, r=empty, i=3
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromEntityPtr, Operation::ResetRefFromEmpty,
       Operation::ValidateRefEmpty, Operation::RemoveEntity1,
       Operation::ValidateRefEmpty},
      // Testcase 173, i=reg_entity, r=nullptr, i=0
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromEntityPtr, Operation::ResetRefFromNullptr,
       Operation::ValidateRefEmpty, Operation::RemoveEntity1,
       Operation::ValidateRefEmpty},
      // Testcase 174, i=reg_entity, r=nullptr, i=1
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromEntityPtr, Operation::ResetRefFromNullptr,
       Operation::ValidateRefEmpty, Operation::RemoveEntity1,
       Operation::ValidateRefEmpty},
      // Testcase 175, i=reg_entity, r=nullptr, i=2
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromEntityPtr, Operation::ResetRefFromNullptr,
       Operation::ValidateRefEmpty, Operation::RemoveEntity1,
       Operation::ValidateRefEmpty},
      // Testcase 176, i=reg_entity, r=nullptr, i=3
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromEntityPtr, Operation::ResetRefFromNullptr,
       Operation::ValidateRefEmpty, Operation::RemoveEntity1,
       Operation::ValidateRefEmpty},
      // Testcase 177, i=reg_entity, r=empty_id, i=0
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromEntityPtr, Operation::ResetRefFromEmptyId,
       Operation::ValidateRefEmpty, Operation::RemoveEntity1,
       Operation::ValidateRefEmpty},
      // Testcase 178, i=reg_entity, r=empty_id, i=1
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromEntityPtr, Operation::ResetRefFromEmptyId,
       Operation::ValidateRefEmpty, Operation::RemoveEntity1,
       Operation::ValidateRefEmpty},
      // Testcase 179, i=reg_entity, r=empty_id, i=2
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromEntityPtr, Operation::ResetRefFromEmptyId,
       Operation::ValidateRefEmpty, Operation::RemoveEntity1,
       Operation::ValidateRefEmpty},
      // Testcase 180, i=reg_entity, r=empty_id, i=3
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromEntityPtr, Operation::ResetRefFromEmptyId,
       Operation::ValidateRefEmpty, Operation::RemoveEntity1,
       Operation::ValidateRefEmpty},
      // Testcase 181, i=reg_entity, r=id, i=0
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromEntityPtr, Operation::CreateEntity2,
       Operation::ResetRefFromId, Operation::RefGetPtr,
       Operation::ValidateRefToEntity2, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity1, Operation::RemoveEntity2,
       Operation::ValidateRefEmpty},
      // Testcase 182, i=reg_entity, r=id, i=1
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromEntityPtr, Operation::CreateEntity2,
       Operation::ResetRefFromId, Operation::RefFillPtr,
       Operation::ValidateRefToEntity2, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity1, Operation::RemoveEntity2,
       Operation::ValidateRefEmpty},
      // Testcase 183, i=reg_entity, r=id, i=2
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromEntityPtr, Operation::CreateEntity2,
       Operation::ResetRefFromId, Operation::ValidateRefToEntity2IdOnly,
       Operation::RemoveEntity1, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2IdOnly},
      // Testcase 184, i=reg_entity, r=id, i=3
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromEntityPtr, Operation::CreateEntity2,
       Operation::ResetRefFromId, Operation::ValidateRefToEntity2IdOnly,
       Operation::RemoveEntity1, Operation::RemoveEntity2,
       Operation::ValidateRefToEntity2IdOnly},
      // Testcase 185, i=reg_entity, r=reg_id, i=0
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromEntityPtr, Operation::CreateEntity2,
       Operation::MgrProcessChanges, Operation::ResetRefFromId,
       Operation::RefGetPtr, Operation::ValidateRefToEntity2,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity1,
       Operation::RemoveEntity2, Operation::ValidateRefToEntity2,
       Operation::MgrProcessChanges, Operation::ValidateRefEmpty},
      // Testcase 186, i=reg_entity, r=reg_id, i=1
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromEntityPtr, Operation::CreateEntity2,
       Operation::MgrProcessChanges, Operation::ResetRefFromId,
       Operation::RefFillPtr, Operation::ValidateRefToEntity2,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity1,
       Operation::RemoveEntity2, Operation::ValidateRefToEntity2,
       Operation::MgrProcessChanges, Operation::ValidateRefEmpty},
      // Testcase 187, i=reg_entity, r=reg_id, i=2
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromEntityPtr, Operation::CreateEntity2,
       Operation::MgrProcessChanges, Operation::ResetRefFromId,
       Operation::ValidateRefToEntity2IdOnly, Operation::RemoveEntity1,
       Operation::RemoveEntity2, Operation::ValidateRefToEntity2IdOnly},
      // Testcase 188, i=reg_entity, r=reg_id, i=3
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromEntityPtr, Operation::CreateEntity2,
       Operation::MgrProcessChanges, Operation::ResetRefFromId,
       Operation::ValidateRefToEntity2IdOnly, Operation::RemoveEntity1,
       Operation::RemoveEntity2, Operation::ValidateRefToEntity2IdOnly},
      // Testcase 189, i=reg_entity, r=entity, i=0
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromEntityPtr, Operation::CreateEntity2,
       Operation::ResetRefFromEntityPtr, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity1, Operation::RemoveEntity2,
       Operation::ValidateRefEmpty},
      // Testcase 190, i=reg_entity, r=entity, i=1
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromEntityPtr, Operation::CreateEntity2,
       Operation::ResetRefFromEntityPtr, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity1, Operation::RemoveEntity2,
       Operation::ValidateRefEmpty},
      // Testcase 191, i=reg_entity, r=entity, i=2
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromEntityPtr, Operation::CreateEntity2,
       Operation::ResetRefFromEntityPtr, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity1, Operation::RemoveEntity2,
       Operation::ValidateRefEmpty},
      // Testcase 192, i=reg_entity, r=entity, i=3
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromEntityPtr, Operation::CreateEntity2,
       Operation::ResetRefFromEntityPtr, Operation::ValidateRefToEntity2,
       Operation::RemoveEntity1, Operation::RemoveEntity2,
       Operation::ValidateRefEmpty},
      // Testcase 193, i=reg_entity, r=reg_entity, i=0
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromEntityPtr, Operation::CreateEntity2,
       Operation::MgrProcessChanges, Operation::ResetRefFromEntityPtr,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity1,
       Operation::RemoveEntity2, Operation::ValidateRefToEntity2,
       Operation::MgrProcessChanges, Operation::ValidateRefEmpty},
      // Testcase 194, i=reg_entity, r=reg_entity, i=1
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromEntityPtr, Operation::CreateEntity2,
       Operation::MgrProcessChanges, Operation::ResetRefFromEntityPtr,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity1,
       Operation::RemoveEntity2, Operation::ValidateRefToEntity2,
       Operation::MgrProcessChanges, Operation::ValidateRefEmpty},
      // Testcase 195, i=reg_entity, r=reg_entity, i=2
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromEntityPtr, Operation::CreateEntity2,
       Operation::MgrProcessChanges, Operation::ResetRefFromEntityPtr,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity1,
       Operation::RemoveEntity2, Operation::ValidateRefToEntity2,
       Operation::MgrProcessChanges, Operation::ValidateRefEmpty},
      // Testcase 196, i=reg_entity, r=reg_entity, i=3
      {Operation::CreateEntity1, Operation::MgrProcessChanges,
       Operation::CreateRefFromEntityPtr, Operation::CreateEntity2,
       Operation::MgrProcessChanges, Operation::ResetRefFromEntityPtr,
       Operation::ValidateRefToEntity2, Operation::RemoveEntity1,
       Operation::RemoveEntity2, Operation::ValidateRefToEntity2,
       Operation::MgrProcessChanges, Operation::ValidateRefEmpty}};

  size_t i = 0;
  for (const auto &order : testcases) {
    ++i;
    BEGIN_TESTCASE("Entity ref test - CASE " + to_string(i)) {
      Entity *entity1 = nullptr;
      Entity *entity2 = nullptr;
      EntityRef *entity_ref = nullptr;

      TC_STATUS("Start to execute " << order.size() << " operations in order.");
      for (Operation op : order) {
        switch (op) {
        case Operation::CreateEntity1:
          entity1 = mgr->new_entity();
          break;
        case Operation::MgrProcessChanges:
          mgr->process_changes();
          break;
        case Operation::CreateRefFromEmpty:
          entity_ref = new EntityRef();
          validate_empty_ref(mgr, *entity_ref);
          break;
        case Operation::CreateRefFromNullptr:
          entity_ref = new EntityRef(nullptr);
          validate_empty_ref(mgr, *entity_ref);
          break;
        case Operation::CreateRefFromEmptyId:
          entity_ref = new EntityRef(Entity::EMPTY_ID);
          validate_empty_ref(mgr, *entity_ref);
          break;
        case Operation::CreateRefFromId:
          entity_ref = new EntityRef(entity1->get_id());
          validate_ref_id_only(mgr, *entity_ref, entity1);
          break;
        case Operation::CreateRefFromEntityPtr:
          entity_ref = new EntityRef(entity1);
          validate_ref(mgr, *entity_ref, entity1);
          break;
        case Operation::CreateEntity2:
          entity2 = mgr->new_entity();
          break;

        case Operation::ResetRefFromEmpty:
          entity_ref->reset();
          validate_empty_ref(mgr, *entity_ref);
          break;
        case Operation::ResetRefFromNullptr:
          entity_ref->reset(nullptr);
          validate_empty_ref(mgr, *entity_ref);
          break;
        case Operation::ResetRefFromEmptyId:
          entity_ref->reset(Entity::EMPTY_ID);
          validate_empty_ref(mgr, *entity_ref);
          break;
        case Operation::ResetRefFromId:
          entity_ref->reset(entity2->get_id());
          validate_ref_id_only(mgr, *entity_ref, entity2);
          break;
        case Operation::ResetRefFromEntityPtr:
          entity_ref->reset(entity2);
          validate_ref(mgr, *entity_ref, entity2);
          break;

        case Operation::RefGetPtr: {
          Entity *ptr = entity_ref->get_ptr(mgr);
          TC_REQUIRE_EQ(ptr, entity2);
          validate_ref(mgr, *entity_ref, entity2);
          break;
        }

        case Operation::RefFillPtr:
          entity_ref->fill_ptr(mgr);
          validate_ref(mgr, *entity_ref, entity2);
          break;

        case Operation::RemoveEntity1:
          entity1->remove();
          break;
        case Operation::RemoveEntity2:
          entity2->remove();
          break;
        case Operation::ValidateRefEmpty:
          validate_empty_ref(mgr, *entity_ref);
          break;
        case Operation::ValidateRefToEntity2:
          validate_ref(mgr, *entity_ref, entity2);
          break;
        case Operation::ValidateRefToEntity2IdOnly:
          validate_ref_id_only(mgr, *entity_ref, entity2);
          break;
        }
      }
    }
    END_TESTCASE;
  }
}