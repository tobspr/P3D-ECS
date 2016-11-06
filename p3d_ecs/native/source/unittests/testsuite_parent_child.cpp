
#include "testsuite_parent_child.h"

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
testsuite_parent_child() {

  enum class Operation { ConstructRoot, ConstructChild, ReparentChild, Update, DeleteRoot, DeleteChild };

  // see unittest/generate_tc1_code.py
  vector<vector<Operation>> testcases = {
    { Operation::ConstructRoot, Operation::Update, Operation::ConstructChild, Operation::Update,
      Operation::ReparentChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 1
    { Operation::ConstructRoot, Operation::ConstructChild, Operation::Update, Operation::ReparentChild,
      Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 2
    { Operation::ConstructRoot, Operation::Update, Operation::ConstructChild, Operation::ReparentChild,
      Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 3
    { Operation::ConstructRoot, Operation::ConstructChild, Operation::ReparentChild, Operation::Update,
      Operation::DeleteRoot, Operation::Update }, // Testcase 4
    { Operation::ConstructChild, Operation::Update, Operation::ConstructRoot, Operation::Update,
      Operation::ReparentChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 5
    { Operation::ConstructChild, Operation::ConstructRoot, Operation::Update, Operation::ReparentChild,
      Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 6
    { Operation::ConstructChild, Operation::Update, Operation::ConstructRoot, Operation::ReparentChild,
      Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 7
    { Operation::ConstructChild, Operation::ConstructRoot, Operation::ReparentChild, Operation::Update,
      Operation::DeleteRoot, Operation::Update }, // Testcase 8
    { Operation::ConstructRoot, Operation::Update, Operation::ConstructChild, Operation::Update,
      Operation::ReparentChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 9
    { Operation::ConstructRoot, Operation::ConstructChild, Operation::Update, Operation::ReparentChild,
      Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 10
    { Operation::ConstructRoot, Operation::Update, Operation::ConstructChild, Operation::ReparentChild,
      Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 11
    { Operation::ConstructRoot, Operation::ConstructChild, Operation::ReparentChild, Operation::Update,
      Operation::DeleteRoot, Operation::Update }, // Testcase 12
    { Operation::ConstructChild, Operation::Update, Operation::ConstructRoot, Operation::Update,
      Operation::ReparentChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 13
    { Operation::ConstructChild, Operation::ConstructRoot, Operation::Update, Operation::ReparentChild,
      Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 14
    { Operation::ConstructChild, Operation::Update, Operation::ConstructRoot, Operation::ReparentChild,
      Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 15
    { Operation::ConstructChild, Operation::ConstructRoot, Operation::ReparentChild, Operation::Update,
      Operation::DeleteRoot, Operation::Update }, // Testcase 16
    { Operation::ConstructRoot, Operation::Update, Operation::ConstructChild, Operation::Update,
      Operation::ReparentChild, Operation::Update, Operation::DeleteChild, Operation::Update, Operation::DeleteRoot,
      Operation::Update }, // Testcase 17
    { Operation::ConstructRoot, Operation::ConstructChild, Operation::Update, Operation::ReparentChild,
      Operation::Update, Operation::DeleteChild, Operation::Update, Operation::DeleteRoot,
      Operation::Update }, // Testcase 18
    { Operation::ConstructRoot, Operation::Update, Operation::ConstructChild, Operation::ReparentChild,
      Operation::Update, Operation::DeleteChild, Operation::Update, Operation::DeleteRoot,
      Operation::Update }, // Testcase 19
    { Operation::ConstructRoot, Operation::ConstructChild, Operation::ReparentChild, Operation::Update,
      Operation::DeleteChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 20
    { Operation::ConstructChild, Operation::Update, Operation::ConstructRoot, Operation::Update,
      Operation::ReparentChild, Operation::Update, Operation::DeleteChild, Operation::Update, Operation::DeleteRoot,
      Operation::Update }, // Testcase 21
    { Operation::ConstructChild, Operation::ConstructRoot, Operation::Update, Operation::ReparentChild,
      Operation::Update, Operation::DeleteChild, Operation::Update, Operation::DeleteRoot,
      Operation::Update }, // Testcase 22
    { Operation::ConstructChild, Operation::Update, Operation::ConstructRoot, Operation::ReparentChild,
      Operation::Update, Operation::DeleteChild, Operation::Update, Operation::DeleteRoot,
      Operation::Update }, // Testcase 23
    { Operation::ConstructChild, Operation::ConstructRoot, Operation::ReparentChild, Operation::Update,
      Operation::DeleteChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 24
    { Operation::ConstructRoot, Operation::Update, Operation::ConstructChild, Operation::Update,
      Operation::ReparentChild, Operation::Update, Operation::DeleteChild, Operation::DeleteRoot,
      Operation::Update }, // Testcase 25
    { Operation::ConstructRoot, Operation::ConstructChild, Operation::Update, Operation::ReparentChild,
      Operation::Update, Operation::DeleteChild, Operation::DeleteRoot, Operation::Update }, // Testcase 26
    { Operation::ConstructRoot, Operation::Update, Operation::ConstructChild, Operation::ReparentChild,
      Operation::Update, Operation::DeleteChild, Operation::DeleteRoot, Operation::Update }, // Testcase 27
    { Operation::ConstructRoot, Operation::ConstructChild, Operation::ReparentChild, Operation::Update,
      Operation::DeleteChild, Operation::DeleteRoot, Operation::Update }, // Testcase 28
    { Operation::ConstructChild, Operation::Update, Operation::ConstructRoot, Operation::Update,
      Operation::ReparentChild, Operation::Update, Operation::DeleteChild, Operation::DeleteRoot,
      Operation::Update }, // Testcase 29
    { Operation::ConstructChild, Operation::ConstructRoot, Operation::Update, Operation::ReparentChild,
      Operation::Update, Operation::DeleteChild, Operation::DeleteRoot, Operation::Update }, // Testcase 30
    { Operation::ConstructChild, Operation::Update, Operation::ConstructRoot, Operation::ReparentChild,
      Operation::Update, Operation::DeleteChild, Operation::DeleteRoot, Operation::Update }, // Testcase 31
    { Operation::ConstructChild, Operation::ConstructRoot, Operation::ReparentChild, Operation::Update,
      Operation::DeleteChild, Operation::DeleteRoot, Operation::Update }, // Testcase 32
    { Operation::ConstructRoot, Operation::Update, Operation::ConstructChild, Operation::Update,
      Operation::ReparentChild, Operation::DeleteRoot, Operation::Update }, // Testcase 33
    { Operation::ConstructRoot, Operation::ConstructChild, Operation::Update, Operation::ReparentChild,
      Operation::DeleteRoot, Operation::Update }, // Testcase 34
    { Operation::ConstructRoot, Operation::Update, Operation::ConstructChild, Operation::ReparentChild,
      Operation::DeleteRoot, Operation::Update }, // Testcase 35
    { Operation::ConstructRoot, Operation::ConstructChild, Operation::ReparentChild, Operation::DeleteRoot,
      Operation::Update }, // Testcase 36
    { Operation::ConstructChild, Operation::Update, Operation::ConstructRoot, Operation::Update,
      Operation::ReparentChild, Operation::DeleteRoot, Operation::Update }, // Testcase 37
    { Operation::ConstructChild, Operation::ConstructRoot, Operation::Update, Operation::ReparentChild,
      Operation::DeleteRoot, Operation::Update }, // Testcase 38
    { Operation::ConstructChild, Operation::Update, Operation::ConstructRoot, Operation::ReparentChild,
      Operation::DeleteRoot, Operation::Update }, // Testcase 39
    { Operation::ConstructChild, Operation::ConstructRoot, Operation::ReparentChild, Operation::DeleteRoot,
      Operation::Update }, // Testcase 40
    { Operation::ConstructRoot, Operation::Update, Operation::ConstructChild, Operation::Update,
      Operation::ReparentChild, Operation::DeleteRoot, Operation::Update }, // Testcase 41
    { Operation::ConstructRoot, Operation::ConstructChild, Operation::Update, Operation::ReparentChild,
      Operation::DeleteRoot, Operation::Update }, // Testcase 42
    { Operation::ConstructRoot, Operation::Update, Operation::ConstructChild, Operation::ReparentChild,
      Operation::DeleteRoot, Operation::Update }, // Testcase 43
    { Operation::ConstructRoot, Operation::ConstructChild, Operation::ReparentChild, Operation::DeleteRoot,
      Operation::Update }, // Testcase 44
    { Operation::ConstructChild, Operation::Update, Operation::ConstructRoot, Operation::Update,
      Operation::ReparentChild, Operation::DeleteRoot, Operation::Update }, // Testcase 45
    { Operation::ConstructChild, Operation::ConstructRoot, Operation::Update, Operation::ReparentChild,
      Operation::DeleteRoot, Operation::Update }, // Testcase 46
    { Operation::ConstructChild, Operation::Update, Operation::ConstructRoot, Operation::ReparentChild,
      Operation::DeleteRoot, Operation::Update }, // Testcase 47
    { Operation::ConstructChild, Operation::ConstructRoot, Operation::ReparentChild, Operation::DeleteRoot,
      Operation::Update }, // Testcase 48
    { Operation::ConstructRoot, Operation::Update, Operation::ConstructChild, Operation::Update,
      Operation::ReparentChild, Operation::DeleteChild, Operation::Update, Operation::DeleteRoot,
      Operation::Update }, // Testcase 49
    { Operation::ConstructRoot, Operation::ConstructChild, Operation::Update, Operation::ReparentChild,
      Operation::DeleteChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 50
    { Operation::ConstructRoot, Operation::Update, Operation::ConstructChild, Operation::ReparentChild,
      Operation::DeleteChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 51
    { Operation::ConstructRoot, Operation::ConstructChild, Operation::ReparentChild, Operation::DeleteChild,
      Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 52
    { Operation::ConstructChild, Operation::Update, Operation::ConstructRoot, Operation::Update,
      Operation::ReparentChild, Operation::DeleteChild, Operation::Update, Operation::DeleteRoot,
      Operation::Update }, // Testcase 53
    { Operation::ConstructChild, Operation::ConstructRoot, Operation::Update, Operation::ReparentChild,
      Operation::DeleteChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 54
    { Operation::ConstructChild, Operation::Update, Operation::ConstructRoot, Operation::ReparentChild,
      Operation::DeleteChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 55
    { Operation::ConstructChild, Operation::ConstructRoot, Operation::ReparentChild, Operation::DeleteChild,
      Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 56
    { Operation::ConstructRoot, Operation::Update, Operation::ConstructChild, Operation::Update,
      Operation::ReparentChild, Operation::DeleteChild, Operation::DeleteRoot, Operation::Update }, // Testcase 57
    { Operation::ConstructRoot, Operation::ConstructChild, Operation::Update, Operation::ReparentChild,
      Operation::DeleteChild, Operation::DeleteRoot, Operation::Update }, // Testcase 58
    { Operation::ConstructRoot, Operation::Update, Operation::ConstructChild, Operation::ReparentChild,
      Operation::DeleteChild, Operation::DeleteRoot, Operation::Update }, // Testcase 59
    { Operation::ConstructRoot, Operation::ConstructChild, Operation::ReparentChild, Operation::DeleteChild,
      Operation::DeleteRoot, Operation::Update }, // Testcase 60
    { Operation::ConstructChild, Operation::Update, Operation::ConstructRoot, Operation::Update,
      Operation::ReparentChild, Operation::DeleteChild, Operation::DeleteRoot, Operation::Update }, // Testcase 61
    { Operation::ConstructChild, Operation::ConstructRoot, Operation::Update, Operation::ReparentChild,
      Operation::DeleteChild, Operation::DeleteRoot, Operation::Update }, // Testcase 62
    { Operation::ConstructChild, Operation::Update, Operation::ConstructRoot, Operation::ReparentChild,
      Operation::DeleteChild, Operation::DeleteRoot, Operation::Update }, // Testcase 63
    { Operation::ConstructChild, Operation::ConstructRoot, Operation::ReparentChild, Operation::DeleteChild,
      Operation::DeleteRoot, Operation::Update }, // Testcase 64

  };

  size_t i = 0;
  for (const auto& order : testcases) {
    ++i;
    BEGIN_TESTCASE("Parent Child Relations - CASE " + to_string(i)) {

      auto make_entity = [&]() {
        Entity* tmp = mgr->new_entity();
        tmp->new_component<TransformComponent>();
        return tmp;
      };

      Entity* root = nullptr;
      Entity* child = nullptr;

      TC_STATUS("Start to execute " << order.size() << " operations in order.");
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
    }
    END_TESTCASE;
  }
};
