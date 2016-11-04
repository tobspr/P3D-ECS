
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

void testcase_serialization() 
{
    general_testsuite("Empty ref test: EntityRef()", [&](EntityManager *mgr) {
        EntityRef ref;

        TC_EXPECT(ref.get_id(), Entity::EMPTY_ID);
        TC_EXPECT(ref.get_ptr(mgr), nullptr);
        TC_EXPECT(ref.is_empty(), true);
        TC_EXPECT(ref.has_access(), false);
        TC_EXPECT(ref.fill_ptr(mgr), true);
        TC_EXPECT(ref.operator->(), nullptr);
        TC_EXPECT((Entity*)ref, nullptr);
    });

    general_testsuite("Empty ref test: EntityRef(Entity::id_t)", [&](EntityManager *mgr) {
        EntityRef ref(ENtity::EMPTY_ID);

        TC_EXPECT(ref.get_id(), Entity::EMPTY_ID);
        TC_EXPECT(ref.get_ptr(mgr), nullptr);
        TC_EXPECT(ref.is_empty(), true);
        TC_EXPECT(ref.has_access(), false);
        TC_EXPECT(ref.fill_ptr(mgr), true);
        TC_EXPECT(ref.operator->(), nullptr);
        TC_EXPECT((Entity*)ref, nullptr);
    });

    general_testsuite("Empty ref test: EntityRef(Entity*)", [&](EntityManager *mgr) {
        EntityRef ref(nullptr);

        TC_EXPECT(ref.get_id(), Entity::EMPTY_ID);
        TC_EXPECT(ref.get_ptr(mgr), nullptr);
        TC_EXPECT(ref.is_empty(), true);
        TC_EXPECT(ref.has_access(), false);
        TC_EXPECT(ref.fill_ptr(mgr), true);
        TC_EXPECT(ref.operator->(), nullptr);
        TC_EXPECT((Entity*)ref, nullptr);
    });



}