
#include "testcase_entityref.h"

#include "unittest.h"
#include "entity_ref.h"

#include <iostream>
#include <functional>
#include <string>
#include <fstream>
#include <sstream>
#include <initializer_list>
using namespace std;

void testcase_entityref() 
{
	auto validate_empty_ref = [&](EntityManager *mgr, EntityRef& ref) {
        TC_EXPECT(ref.get_id(), Entity::EMPTY_ID);
        TC_EXPECT(ref.get_ptr(mgr), nullptr);
        TC_EXPECT(ref.is_empty(), true);
        TC_EXPECT(ref.has_access(), false);
        TC_EXPECT(ref.fill_ptr(mgr), true);
        TC_EXPECT(ref.operator->(), nullptr);
        TC_EXPECT((Entity*)ref, nullptr);
    };

    auto validate_ref = [&](EntityManager* mgr, EntityRef& ref, Entity* expected) {
        TC_EXPECT(ref.get_id(), expected->get_id());
        TC_EXPECT(ref.get_ptr(mgr), expected);
        TC_EXPECT(ref.is_empty(), false);
        TC_EXPECT(ref.has_access(), true);
        TC_EXPECT(ref.fill_ptr(mgr), true);
        TC_EXPECT(ref.operator->(), expected);
        TC_EXPECT((Entity*)ref, expected);
    };

    general_testsuite("Empty ref test: EntityRef()", [&](EntityManager *mgr) {
        EntityRef ref;
        validate_empty_ref(mgr, ref);
    });

    general_testsuite("Empty ref test: EntityRef(Entity::id_t)", [&](EntityManager *mgr) {
        EntityRef ref(Entity::EMPTY_ID);
        validate_empty_ref(mgr, ref);
    });

    general_testsuite("Empty ref test: EntityRef(Entity*)", [&](EntityManager *mgr) {
        EntityRef ref(nullptr);
        validate_empty_ref(mgr, ref);
    });

    general_testsuite("Simple ref test w/o register", [&](EntityManager *mgr) {
        Entity* entity = mgr->new_entity();
        EntityRef ref(entity);
        validate_ref(mgr, ref, entity);

        entity->remove();
        // remove is processed directly, because the entity has not been registered yet
        validate_empty_ref(mgr, ref);
    });

    
    general_testsuite("Simple ref test w/ register", [&](EntityManager *mgr) {
        Entity* entity = mgr->new_entity();
        EntityRef ref(entity);
        validate_ref(mgr, ref, entity);

        mgr->process_changes();

        validate_ref(mgr, ref, entity);

        entity->remove();
        // remove is not processed directly, because the entity has been registered
        validate_ref(mgr, ref, entity);

        mgr->process_changes();

        validate_empty_ref(mgr, ref);
    });


}