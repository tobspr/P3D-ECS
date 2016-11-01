# P3D-ECS
Panda3D Entity Component System

This is an experiment with entity component systems, and an attempt to connect
them with Panda3D. This is by no means a finished project.

Example:

```cpp

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

    cout << "TC> Constructing another entity" << endl;
    Entity* entity2 = mgr->new_entity();
    entity2->add<TransformComponent>(LVecBase3f(), LVecBase3f(), LVecBase3f());
    entity2->add<PhysicsComponent>(0.0, LVecBase2f());

    cout << "TC> Constructing new movement system" << endl;
    MovementSystem* sys = mgr->new_system<MovementSystem>();

    auto update = [&](){
        cout << "TC> Single step .." << endl;
        float dt = 0.05;
        mgr->single_step(dt);

        cout << "TC> Update system.. " << endl;
        sys->process(dt);
    };
    
    update();

    cout << "TC> Modifying first entry so it also has a pyhsics component" << endl;
    entity->add<PhysicsComponent>(0.0, LVecBase2f());

    update();

    cout << "TC> Done." << endl;
```