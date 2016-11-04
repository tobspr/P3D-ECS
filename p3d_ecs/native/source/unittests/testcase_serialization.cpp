
#include "testcase_serialization.h"

#include "unittest.h"
#include "serialization.h"

#include <iostream>
#include <functional>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

void testcase_serialization() 
{

    
    general_testsuite("Serializing a component", [&](EntityManager *mgr) {

        Entity* entity = mgr->new_entity();

        TransformComponent& transform = entity->new_component<TransformComponent>();

        YAMLSerializer serializer ;
        transform.serialize(&serializer);

        cout << serializer.get_output() << endl;

        entity->remove();

    });

}