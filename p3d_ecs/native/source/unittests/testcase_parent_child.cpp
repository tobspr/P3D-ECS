
#include "testcase_parent_child.h"

#include "movement_system.h"
#include "unittest.h"

#include <iostream>
#include <unordered_set>
#include <functional>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

/*
void testcase_1() {
  general_testsuite("Testcase 1", [](EntityManager *mgr) {

    cout << "\n\nTC> Constructing new entity" << endl;
    Entity *entity = mgr->new_entity();

    cout << "\n\nTC> Adding transform component" << endl;
    entity->new_component<TransformComponent>(
        LVecBase3f(1, 2, 3), LVecBase3f(2, 3, 4), LVecBase3f(5, 6, 7));

    cout << "\n\nTC> Modifying transform component" << endl;
    entity->get_component<TransformComponent>().set_pos(LVecBase3f(5, 5, 6));

    cout << "\n\nTC> Constructing another entity" << endl;
    Entity *entity2 = mgr->new_entity();
    entity2->new_component<TransformComponent>();
    entity2->new_component<PhysicsComponent>();

    cout << "\n\nTC> Constructing new movement system" << endl;
    MovementSystem *sys = mgr->new_system<MovementSystem>();

    auto update = [&]() {
      cout << "\n\nTC> Single step .." << endl;
      float dt = 0.05;
      mgr->process_changes();

      cout << "\n\nTC> Update system.. " << endl;
      sys->process(dt);
    };

    update();

    cout << "\n\nTC> Modifying first entry so it also has a pyhsics component"
         << endl;
    entity->new_component<PhysicsComponent>(0.0, LVecBase2f());

    update();

    cout << "\n\nTC> Constructing new entity just to delete it " << endl;
    Entity *entity3 = mgr->new_entity();
    entity3->remove();

    update();

    cout << "\n\nTC> Constructing new entity to delete it afterwards" << endl;
    Entity *entity4 = mgr->new_entity();
    entity4->new_component<TransformComponent>();
    entity4->new_component<PhysicsComponent>();

    update();
    entity4->remove();
    update();

    cout << "\n\nTC> Constructing new entity just before shutdown" << endl;
    Entity *entity5 = mgr->new_entity();

    cout << "\n\nTC> Deleting system" << endl;
    delete sys;

  });
};
*/

void testcase_parent_child() {

  enum class Operation {
    ConstructRoot,
    ConstructChild,
    ReparentChild,
    Update,
    DeleteRoot,
    DeleteChild
  };

  // see unittest/generate_tc1_code.py
  vector<vector<Operation>> testcases = {
      {Operation::ConstructRoot, Operation::Update, Operation::ConstructChild,
       Operation::Update, Operation::ReparentChild, Operation::Update,
       Operation::DeleteRoot, Operation::Update}, // Testcase 1
      {Operation::ConstructRoot, Operation::ConstructChild, Operation::Update,
       Operation::ReparentChild, Operation::Update, Operation::DeleteRoot,
       Operation::Update}, // Testcase 2
      {Operation::ConstructRoot, Operation::Update, Operation::ConstructChild,
       Operation::ReparentChild, Operation::Update, Operation::DeleteRoot,
       Operation::Update}, // Testcase 3
      {Operation::ConstructRoot, Operation::ConstructChild,
       Operation::ReparentChild, Operation::Update, Operation::DeleteRoot,
       Operation::Update}, // Testcase 4
      {Operation::ConstructChild, Operation::Update, Operation::ConstructRoot,
       Operation::Update, Operation::ReparentChild, Operation::Update,
       Operation::DeleteRoot, Operation::Update}, // Testcase 5
      {Operation::ConstructChild, Operation::ConstructRoot, Operation::Update,
       Operation::ReparentChild, Operation::Update, Operation::DeleteRoot,
       Operation::Update}, // Testcase 6
      {Operation::ConstructChild, Operation::Update, Operation::ConstructRoot,
       Operation::ReparentChild, Operation::Update, Operation::DeleteRoot,
       Operation::Update}, // Testcase 7
      {Operation::ConstructChild, Operation::ConstructRoot,
       Operation::ReparentChild, Operation::Update, Operation::DeleteRoot,
       Operation::Update}, // Testcase 8
      {Operation::ConstructRoot, Operation::Update, Operation::ConstructChild,
       Operation::Update, Operation::ReparentChild, Operation::Update,
       Operation::DeleteRoot, Operation::Update}, // Testcase 9
      {Operation::ConstructRoot, Operation::ConstructChild, Operation::Update,
       Operation::ReparentChild, Operation::Update, Operation::DeleteRoot,
       Operation::Update}, // Testcase 10
      {Operation::ConstructRoot, Operation::Update, Operation::ConstructChild,
       Operation::ReparentChild, Operation::Update, Operation::DeleteRoot,
       Operation::Update}, // Testcase 11
      {Operation::ConstructRoot, Operation::ConstructChild,
       Operation::ReparentChild, Operation::Update, Operation::DeleteRoot,
       Operation::Update}, // Testcase 12
      {Operation::ConstructChild, Operation::Update, Operation::ConstructRoot,
       Operation::Update, Operation::ReparentChild, Operation::Update,
       Operation::DeleteRoot, Operation::Update}, // Testcase 13
      {Operation::ConstructChild, Operation::ConstructRoot, Operation::Update,
       Operation::ReparentChild, Operation::Update, Operation::DeleteRoot,
       Operation::Update}, // Testcase 14
      {Operation::ConstructChild, Operation::Update, Operation::ConstructRoot,
       Operation::ReparentChild, Operation::Update, Operation::DeleteRoot,
       Operation::Update}, // Testcase 15
      {Operation::ConstructChild, Operation::ConstructRoot,
       Operation::ReparentChild, Operation::Update, Operation::DeleteRoot,
       Operation::Update}, // Testcase 16
      {Operation::ConstructRoot, Operation::Update, Operation::ConstructChild,
       Operation::Update, Operation::ReparentChild, Operation::Update,
       Operation::DeleteChild, Operation::Update, Operation::DeleteRoot,
       Operation::Update}, // Testcase 17
      {Operation::ConstructRoot, Operation::ConstructChild, Operation::Update,
       Operation::ReparentChild, Operation::Update, Operation::DeleteChild,
       Operation::Update, Operation::DeleteRoot,
       Operation::Update}, // Testcase 18
      {Operation::ConstructRoot, Operation::Update, Operation::ConstructChild,
       Operation::ReparentChild, Operation::Update, Operation::DeleteChild,
       Operation::Update, Operation::DeleteRoot,
       Operation::Update}, // Testcase 19
      {Operation::ConstructRoot, Operation::ConstructChild,
       Operation::ReparentChild, Operation::Update, Operation::DeleteChild,
       Operation::Update, Operation::DeleteRoot,
       Operation::Update}, // Testcase 20
      {Operation::ConstructChild, Operation::Update, Operation::ConstructRoot,
       Operation::Update, Operation::ReparentChild, Operation::Update,
       Operation::DeleteChild, Operation::Update, Operation::DeleteRoot,
       Operation::Update}, // Testcase 21
      {Operation::ConstructChild, Operation::ConstructRoot, Operation::Update,
       Operation::ReparentChild, Operation::Update, Operation::DeleteChild,
       Operation::Update, Operation::DeleteRoot,
       Operation::Update}, // Testcase 22
      {Operation::ConstructChild, Operation::Update, Operation::ConstructRoot,
       Operation::ReparentChild, Operation::Update, Operation::DeleteChild,
       Operation::Update, Operation::DeleteRoot,
       Operation::Update}, // Testcase 23
      {Operation::ConstructChild, Operation::ConstructRoot,
       Operation::ReparentChild, Operation::Update, Operation::DeleteChild,
       Operation::Update, Operation::DeleteRoot,
       Operation::Update}, // Testcase 24
      {Operation::ConstructRoot, Operation::Update, Operation::ConstructChild,
       Operation::Update, Operation::ReparentChild, Operation::Update,
       Operation::DeleteChild, Operation::DeleteRoot,
       Operation::Update}, // Testcase 25
      {Operation::ConstructRoot, Operation::ConstructChild, Operation::Update,
       Operation::ReparentChild, Operation::Update, Operation::DeleteChild,
       Operation::DeleteRoot, Operation::Update}, // Testcase 26
      {Operation::ConstructRoot, Operation::Update, Operation::ConstructChild,
       Operation::ReparentChild, Operation::Update, Operation::DeleteChild,
       Operation::DeleteRoot, Operation::Update}, // Testcase 27
      {Operation::ConstructRoot, Operation::ConstructChild,
       Operation::ReparentChild, Operation::Update, Operation::DeleteChild,
       Operation::DeleteRoot, Operation::Update}, // Testcase 28
      {Operation::ConstructChild, Operation::Update, Operation::ConstructRoot,
       Operation::Update, Operation::ReparentChild, Operation::Update,
       Operation::DeleteChild, Operation::DeleteRoot,
       Operation::Update}, // Testcase 29
      {Operation::ConstructChild, Operation::ConstructRoot, Operation::Update,
       Operation::ReparentChild, Operation::Update, Operation::DeleteChild,
       Operation::DeleteRoot, Operation::Update}, // Testcase 30
      {Operation::ConstructChild, Operation::Update, Operation::ConstructRoot,
       Operation::ReparentChild, Operation::Update, Operation::DeleteChild,
       Operation::DeleteRoot, Operation::Update}, // Testcase 31
      {Operation::ConstructChild, Operation::ConstructRoot,
       Operation::ReparentChild, Operation::Update, Operation::DeleteChild,
       Operation::DeleteRoot, Operation::Update}, // Testcase 32
      {Operation::ConstructRoot, Operation::Update, Operation::ConstructChild,
       Operation::Update, Operation::ReparentChild, Operation::DeleteRoot,
       Operation::Update}, // Testcase 33
      {Operation::ConstructRoot, Operation::ConstructChild, Operation::Update,
       Operation::ReparentChild, Operation::DeleteRoot,
       Operation::Update}, // Testcase 34
      {Operation::ConstructRoot, Operation::Update, Operation::ConstructChild,
       Operation::ReparentChild, Operation::DeleteRoot,
       Operation::Update}, // Testcase 35
      {Operation::ConstructRoot, Operation::ConstructChild,
       Operation::ReparentChild, Operation::DeleteRoot,
       Operation::Update}, // Testcase 36
      {Operation::ConstructChild, Operation::Update, Operation::ConstructRoot,
       Operation::Update, Operation::ReparentChild, Operation::DeleteRoot,
       Operation::Update}, // Testcase 37
      {Operation::ConstructChild, Operation::ConstructRoot, Operation::Update,
       Operation::ReparentChild, Operation::DeleteRoot,
       Operation::Update}, // Testcase 38
      {Operation::ConstructChild, Operation::Update, Operation::ConstructRoot,
       Operation::ReparentChild, Operation::DeleteRoot,
       Operation::Update}, // Testcase 39
      {Operation::ConstructChild, Operation::ConstructRoot,
       Operation::ReparentChild, Operation::DeleteRoot,
       Operation::Update}, // Testcase 40
      {Operation::ConstructRoot, Operation::Update, Operation::ConstructChild,
       Operation::Update, Operation::ReparentChild, Operation::DeleteRoot,
       Operation::Update}, // Testcase 41
      {Operation::ConstructRoot, Operation::ConstructChild, Operation::Update,
       Operation::ReparentChild, Operation::DeleteRoot,
       Operation::Update}, // Testcase 42
      {Operation::ConstructRoot, Operation::Update, Operation::ConstructChild,
       Operation::ReparentChild, Operation::DeleteRoot,
       Operation::Update}, // Testcase 43
      {Operation::ConstructRoot, Operation::ConstructChild,
       Operation::ReparentChild, Operation::DeleteRoot,
       Operation::Update}, // Testcase 44
      {Operation::ConstructChild, Operation::Update, Operation::ConstructRoot,
       Operation::Update, Operation::ReparentChild, Operation::DeleteRoot,
       Operation::Update}, // Testcase 45
      {Operation::ConstructChild, Operation::ConstructRoot, Operation::Update,
       Operation::ReparentChild, Operation::DeleteRoot,
       Operation::Update}, // Testcase 46
      {Operation::ConstructChild, Operation::Update, Operation::ConstructRoot,
       Operation::ReparentChild, Operation::DeleteRoot,
       Operation::Update}, // Testcase 47
      {Operation::ConstructChild, Operation::ConstructRoot,
       Operation::ReparentChild, Operation::DeleteRoot,
       Operation::Update}, // Testcase 48
      {Operation::ConstructRoot, Operation::Update, Operation::ConstructChild,
       Operation::Update, Operation::ReparentChild, Operation::DeleteChild,
       Operation::Update, Operation::DeleteRoot,
       Operation::Update}, // Testcase 49
      {Operation::ConstructRoot, Operation::ConstructChild, Operation::Update,
       Operation::ReparentChild, Operation::DeleteChild, Operation::Update,
       Operation::DeleteRoot, Operation::Update}, // Testcase 50
      {Operation::ConstructRoot, Operation::Update, Operation::ConstructChild,
       Operation::ReparentChild, Operation::DeleteChild, Operation::Update,
       Operation::DeleteRoot, Operation::Update}, // Testcase 51
      {Operation::ConstructRoot, Operation::ConstructChild,
       Operation::ReparentChild, Operation::DeleteChild, Operation::Update,
       Operation::DeleteRoot, Operation::Update}, // Testcase 52
      {Operation::ConstructChild, Operation::Update, Operation::ConstructRoot,
       Operation::Update, Operation::ReparentChild, Operation::DeleteChild,
       Operation::Update, Operation::DeleteRoot,
       Operation::Update}, // Testcase 53
      {Operation::ConstructChild, Operation::ConstructRoot, Operation::Update,
       Operation::ReparentChild, Operation::DeleteChild, Operation::Update,
       Operation::DeleteRoot, Operation::Update}, // Testcase 54
      {Operation::ConstructChild, Operation::Update, Operation::ConstructRoot,
       Operation::ReparentChild, Operation::DeleteChild, Operation::Update,
       Operation::DeleteRoot, Operation::Update}, // Testcase 55
      {Operation::ConstructChild, Operation::ConstructRoot,
       Operation::ReparentChild, Operation::DeleteChild, Operation::Update,
       Operation::DeleteRoot, Operation::Update}, // Testcase 56
      {Operation::ConstructRoot, Operation::Update, Operation::ConstructChild,
       Operation::Update, Operation::ReparentChild, Operation::DeleteChild,
       Operation::DeleteRoot, Operation::Update}, // Testcase 57
      {Operation::ConstructRoot, Operation::ConstructChild, Operation::Update,
       Operation::ReparentChild, Operation::DeleteChild, Operation::DeleteRoot,
       Operation::Update}, // Testcase 58
      {Operation::ConstructRoot, Operation::Update, Operation::ConstructChild,
       Operation::ReparentChild, Operation::DeleteChild, Operation::DeleteRoot,
       Operation::Update}, // Testcase 59
      {Operation::ConstructRoot, Operation::ConstructChild,
       Operation::ReparentChild, Operation::DeleteChild, Operation::DeleteRoot,
       Operation::Update}, // Testcase 60
      {Operation::ConstructChild, Operation::Update, Operation::ConstructRoot,
       Operation::Update, Operation::ReparentChild, Operation::DeleteChild,
       Operation::DeleteRoot, Operation::Update}, // Testcase 61
      {Operation::ConstructChild, Operation::ConstructRoot, Operation::Update,
       Operation::ReparentChild, Operation::DeleteChild, Operation::DeleteRoot,
       Operation::Update}, // Testcase 62
      {Operation::ConstructChild, Operation::Update, Operation::ConstructRoot,
       Operation::ReparentChild, Operation::DeleteChild, Operation::DeleteRoot,
       Operation::Update}, // Testcase 63
      {Operation::ConstructChild, Operation::ConstructRoot,
       Operation::ReparentChild, Operation::DeleteChild, Operation::DeleteRoot,
       Operation::Update}, // Testcase 64

  };

  size_t i = 0;
  for (const auto &order : testcases) {
    ++i;
    general_testsuite(
        "Parent Child Relations - CASE " + to_string(i),
        [&](EntityManager *mgr) {

          auto make_entity = [&]() {
            Entity *tmp = mgr->new_entity();
            tmp->new_component<TransformComponent>();
            return tmp;
          };
          Entity *root = nullptr;
          Entity *child = nullptr;

          TC_STATUS("Start to execute " << order.size()
                                        << " operations in order.");
          for (Operation op : order) {
            switch (op) {
            case Operation::ConstructRoot:
              root = make_entity();
              break;
            case Operation::ConstructChild:
              child = make_entity();
              break;
            case Operation::ReparentChild:
              child->get_component<TransformComponent>().set_parent(root);
              break;
            case Operation::Update:
              mgr->process_changes();
              break;
            case Operation::DeleteRoot:
              root->remove();
              break;
            case Operation::DeleteChild:
              child->remove();
              break;
            }
          }

        });
  }


};
