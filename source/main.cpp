
#include "main.h"


#include "entity_manager.h"
#include "entity_system.h"
#include "all_components.h"
#include "entity.h"

#include <iostream>

using namespace std;

void testcase_1() {
    
    cout << "TC> Starting testcase 1" << endl;

    cout << "TC> Constructing new entity manager" << endl;
    EntityManager* mgr = new EntityManager();

    cout << "TC> Constructing new entity" << endl;
    Entity* entity = mgr->new_entity();
    
    cout << "TC> Adding transform component" << endl;
    entity->add_component<TransformComponent>(
        LVecBase3f(1, 2, 3),
        LVecBase3f(2, 3, 4),
        LVecBase3f(5, 6, 7)
    );

    cout << "TC> Modifying transform component" << endl;
    entity->get_component<TransformComponent>()->pos = LVecBase3f(5, 5, 6);


    cout << "TC> Done." << endl;

};
