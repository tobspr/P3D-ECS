
#include "testcase_serialization.h"

#include "unittest.h"
#include "plain_text_serializer.h"

#include <iostream>
#include <functional>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

void testcase_serialization() {

  general_testsuite("Serializing a component", [&](EntityManager *mgr) {

    Entity *entity = mgr->new_entity();

    TransformComponent &transform = entity->new_component<TransformComponent>();

    PlainTextSerializer serializer;
    transform.serialize(&serializer);

    cout << serializer.get_output() << endl;

    entity->remove();

    mgr->process_changes();
  });

  general_testsuite("Serializing a modified component",
                    [&](EntityManager *mgr) {

                      Entity *entity = mgr->new_entity();

                      TransformComponent &transform =
                          entity->new_component<TransformComponent>();
                      transform.set_pos({1, 2, 3});
                      transform.set_scale({4, 2, 1});
                      transform.set_hpr({30, 60, 120});

                      PlainTextSerializer serializer;
                      transform.serialize(&serializer);

                      cout << serializer.get_output() << endl;

                      entity->remove();

                      mgr->process_changes();

                    });

  general_testsuite("Serializing a component with parent",
                    [&](EntityManager *mgr) {

                      Entity *entity = mgr->new_entity();

                      TransformComponent &transform =
                          entity->new_component<TransformComponent>();

                      Entity *root = mgr->new_entity();
                      TransformComponent &root_transform =
                          root->new_component<TransformComponent>();

                      transform.set_parent(root);

                      PlainTextSerializer serializer;
                      transform.serialize(&serializer);

                      cout << "Child:" << endl;
                      cout << serializer.get_output() << endl;

                      PlainTextSerializer serializer2;
                      root_transform.serialize(&serializer2);
                      cout << "Root:" << endl;
                      cout << serializer2.get_output() << endl;

                      entity->remove();

                      mgr->process_changes();
                    });

  general_testsuite("Serializing a simple entity", [&](EntityManager *mgr) {

    Entity *entity = mgr->new_entity();

    PlainTextSerializer serializer;
    entity->serialize(&serializer);

    cout << serializer.get_output() << endl;

    entity->remove();
    mgr->process_changes();
  });

  general_testsuite("Serializing a multi-comp entity", [&](EntityManager *mgr) {

    Entity *entity = mgr->new_entity();
    TransformComponent& transform = entity->new_component<TransformComponent>();
    PhysicsComponent& physics = entity->new_component<PhysicsComponent>();

    physics.set_mass(15.0);
	physics.set_static(true);
    transform.set_pos({1, 2, 3});
    transform.set_scale({4, 2, 1});
    transform.set_hpr({30, 60, 120});

    PlainTextSerializer serializer;
    entity->serialize(&serializer);

    cout << serializer.get_output() << endl;

    entity->remove();
    mgr->process_changes();
  });
}