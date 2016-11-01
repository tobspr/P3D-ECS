# P3D-ECS
Panda3D Entity Component System

This is an experiment with entity component systems, and an attempt to connect
them with Panda3D. This is by no means a finished project.

Example:

```cpp

    EntityManager* mgr = new EntityManager();

    Entity* entity = mgr->new_entity();
    entity->add<TransformComponent>(
        LVecBase3f(1, 2, 3),
        LVecBase3f(2, 3, 4),
        LVecBase3f(5, 6, 7)
    );

    entity->get<TransformComponent>().pos = LVecBase3f(5, 5, 6);

    Entity* entity2 = mgr->new_entity();
    entity2->add<TransformComponent>(LVecBase3f(), LVecBase3f(), LVecBase3f());
    entity2->add<PhysicsComponent>(0.0, LVecBase2f());

    MovementSystem* sys = mgr->new_system<MovementSystem>();

    float dt = 0.05;
    mgr->single_step(dt);
    sys->process(dt);
```