# P3D-ECS
Panda3D Entity Component System

This is an experiment with entity component systems, and an attempt to connect
them with Panda3D. This is by no means a finished project.

Example:

```cpp

    // System which processes all entities which have at least a PhysicsCompoonent.
    struct MySystem : public SimpleEntitySystem<PhysicsComponent> {
        MySystem(EntityManager* mgr)
            : SimpleEntitySystem<PhysicsComponent>(mgr){};
        virtual void process(float dt) override
        {
		   for (auto entity : get_entities()) {
                // Do something fancy with this entity
            };
        }
    };


    EntityManager* manager = new EntityManager();

    // Construct a new system to process entities
    MySystem* entity_sys = manager->new_system<MySystem>();

    // Construct a new entity with two components
    Entity* entity = manager->new_entity();
    entity->add<TransformComponent>();
    entity->add<PhysicsComponent>();

    // Construct a child entity
    Entity* child_entity = manager->new_entity();
    child_entity->add<TransformComponent>();
    child_entity->set_parent(entity);

    // Modifying components is straightforward
    entity->get<TransformComponent>().set_pos({5, 5, 6});

    // Update loop, the order of the systems is controlled by the user (you!)
    float dt = 0.05;
    manager->single_step(dt);
    entity_sys->process(dt);

```
