
#include "testcase_generic.h"

#include "movement_system.h"
#include "unittest.h"

#include <iostream>
#include <unordered_set>
#include <functional>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;


void testcase_generic() {
  general_testsuite("Removing entity twice - expecting a warning!", [](EntityManager *mgr) {

    Entity *entity = mgr->new_entity();

    entity->remove(); // fine
    entity->remove(); // bad!
    
  });
  

  general_testsuite("Re-Parenting entity with existing parent", [](EntityManager *mgr) {

    Entity *root_1 = mgr->new_entity();
    Entity *root_2 = mgr->new_entity();
    Entity *child = mgr->new_entity();

    TransformComponent& root_1_transform = root_1->new_component<TransformComponent>();
    TransformComponent& root_2_transform = root_2->new_component<TransformComponent>();
    
    TransformComponent& transform = child->new_component<TransformComponent>();
    transform.set_parent(root_1); 

    TC_EXPECT(root_1_transform.get_children().size(), 1);
    TC_EXPECT(root_2_transform.get_children().size(), 0);


    transform.set_parent(root_2);
    TC_EXPECT(root_1_transform.get_children().size(), 0);
    TC_EXPECT(root_2_transform.get_children().size(), 1);

    transform.set_parent(nullptr);
    TC_EXPECT(root_1_transform.get_children().size(), 0);
    TC_EXPECT(root_2_transform.get_children().size(), 0);
    
  });

};
