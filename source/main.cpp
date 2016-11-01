
#include "main.h"

#include "entity_manager.h"
#include "entity_system.h"
#include "all_components.h"
#include "entity.h"

#include "movement_system.h"

#include <iostream>
#include <unordered_set>

using namespace std;

void testcase_1()
{
    cout << "\n\nTC> Starting testcase 1" << endl;

    cout << "\n\nTC> Constructing new entity manager" << endl;
    EntityManager* mgr = new EntityManager();

    cout << "\n\nTC> Constructing new entity" << endl;
    Entity* entity = mgr->new_entity();

    cout << "\n\nTC> Adding transform component" << endl;
    entity->add<TransformComponent>(LVecBase3f(1, 2, 3), LVecBase3f(2, 3, 4),
        LVecBase3f(5, 6, 7));

    cout << "\n\nTC> Modifying transform component" << endl;
    entity->get<TransformComponent>().pos = LVecBase3f(5, 5, 6);

    cout << "\n\nTC> Constructing another entity" << endl;
    Entity* entity2 = mgr->new_entity();
    entity2->add<TransformComponent>(LVecBase3f(), LVecBase3f(), LVecBase3f());
    entity2->add<PhysicsComponent>(0.0, LVecBase2f());

    cout << "\n\nTC> Constructing new movement system" << endl;
    MovementSystem* sys = mgr->new_system<MovementSystem>();

    auto update = [&]() {
        cout << "\n\nTC> Single step .." << endl;
        float dt = 0.05;
        mgr->single_step(dt);

        cout << "\n\nTC> Update system.. " << endl;
        sys->process(dt);
    };

    update();

    cout << "\n\nTC> Modifying first entry so it also has a pyhsics component"
         << endl;
    entity->add<PhysicsComponent>(0.0, LVecBase2f());

    update();

    cout << "\n\nTC> Constructing new entity just to delete it " << endl;
    Entity* entity3 = mgr->new_entity();
    entity3->remove();

    update();

    cout << "\n\nTC> Constructing new entity to delete it afterwards" << endl;
    Entity* entity4 = mgr->new_entity();

    update();
    entity4->remove();
    update();

    cout << "\n\nTC> Constructing new entity just before shutdown" << endl;
    Entity* entity5 = mgr->new_entity();

    cout << "\n\nTC> Done, cleaning up .." << endl;
    mgr->shutdown();

    delete sys;

    ECS_PRINT_LEAKS();
};
