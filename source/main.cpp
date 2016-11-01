
#include "main.h"


#include "entity_manager.h"
#include "entity_system.h"
#include "all_components.h"
#include "entity.h"

#include "movement_system.h"

#include <iostream>
#include <unordered_set>

using namespace std;

void testcase_1() {
    
    cout << "TC> Starting testcase 1" << endl;

    cout << "TC> Constructing new entity manager" << endl;
    EntityManager* mgr = new EntityManager();

    cout << "TC> Constructing new entity" << endl;
    Entity* entity = mgr->new_entity();
    
    cout << "TC> Adding transform component" << endl;
    entity->add<TransformComponent>(
        LVecBase3f(1, 2, 3),
        LVecBase3f(2, 3, 4),
        LVecBase3f(5, 6, 7)
    );

    cout << "TC> Modifying transform component" << endl;
    entity->get<TransformComponent>().pos = LVecBase3f(5, 5, 6);

    cout << "TC> Constructing new movement system" << endl;
    MovementSystem* sys = mgr->new_system<MovementSystem>();

    cout << "TC> Single step .." << endl;
    float dt = 0.05;
    mgr->single_step(dt);

    cout << "TC> Update system.. " << endl;
    sys->process(dt);

    cout << "TC> Done." << endl;

};
