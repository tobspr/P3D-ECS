[![Build Status](https://travis-ci.org/tobspr/P3D-ECS.svg?branch=master)](https://travis-ci.org/tobspr/P3D-ECS)

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
    entity->new_component<TransformComponent>();
    entity->new_component<PhysicsComponent>();

    // Construct a child entity
    Entity* child_entity = manager->new_entity();
    child_entity->new_component<TransformComponent>();
    child_entity->get_component<TransformComponent>().set_parent(entity);

    // Modifying components is straightforward
    entity->get_component<TransformComponent>().set_pos({5, 5, 6});

    // Update loop, the order of the systems is controlled by the user (you!)
    float dt = 0.05;
    manager->process_changes();
    entity_sys->process(dt);

```

## Adding new components

Components are generated from meta files. To add a new component, first create a new meta file, if your 
component is named `MyComponent` then you would have to create `p3d_ecs/component_meta/my_component.py`.

You can then declare your component in your `my_component.py` file:

```python
from p3d_ecs.meta.component_properties import *

class MyComponent(object):

    something = FloatProperty(
        name="Something",
        default=5.0,
        generate_setter=True
    )

    another = VectorProperty(
        name="Rotation",
        dimensions=2,
        init_with="0.5, 1.0",
        generate_setter=True
    )
```

After running `process_meta_files.py`, there are now all required files for your component generated.
You can head over to `p3d_ecs/native/component_meta/my_component_meta.h` to see the generated meta-class,
and you can find your component class in
`p3d_ecs/native/component_impl/my_component.h`.

The meta class is auto generated and should not be modified, but you can add getters, setters and attributes
to your component class, if you need them.

Your generated component class will look like this:

```cpp
#pragma once
// ...

class MyComponent final : public MyComponentMeta {
public:
  DEFINE_COMPONENT(MyComponent);

  // Add your custom getters/setters here

private:

  inline MyComponent(Entity *entity) : superclass(entity){};

  // Add your custom additional attributes here, notice they
  // are not serialized
};
```

Notice that only the members specified in your meta class can be serialized, if you need to serialize
your additional members, consider adding them either to the meta class, or override the serialization methods.


