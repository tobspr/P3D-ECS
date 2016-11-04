
#include "testcase_serialization.h"

#include "unittest.h"
#include "serialization.h"

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

                    });

  general_testsuite("Serializing a component with parent",
                    [&](EntityManager *mgr) {

                      Entity *entity = mgr->new_entity();

                      TransformComponent &transform =
                          entity->new_component<TransformComponent>();
                      
                      Entity* root = mgr->new_entity();
                      root->new_component<TransformComponent>();

                      transform.set_parent(root);

                      PlainTextSerializer serializer;
                      transform.serialize(&serializer);

                      cout << serializer.get_output() << endl;

                      entity->remove();

                    });
}