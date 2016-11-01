#pragma once
#ifndef P3D_ECS_ENTITYSYSTEM_H
#define P3D_ECS_ENTITYSYSTEM_H

#include "config_module.h"

class EntityManager;
class EntityCollector;

class EntitySystem {
public:
    EntitySystem(EntityManager* manager);
    virtual ~EntitySystem();

    virtual void process(float dt) = 0;

protected:
    EntityManager* _manager;
};

#endif