#pragma once
#ifndef P3D_ECS_ENTITYSYSTEM_H
#define P3D_ECS_ENTITYSYSTEM_H

#include "config_module.h"

class EntityManager;
class EntityCollector;

class EntitySystem {
    friend class EntityManager;
public:
    virtual ~EntitySystem();

    virtual void process(float dt) = 0;

protected:

    EntitySystem(EntityManager* manager);
    EntityManager* _manager;
};

template <typename... Args>
class SimpleEntitySystem : public EntitySystem {
public:
    SimpleEntitySystem(EntityManager* manager)
        : EntitySystem(manager)
    {
        _collector = manager->new_collector<Args...>();
    };

    EntityCollector& get_entities() const { return *_collector; };

private:
    EntityCollector* _collector;
};

#endif