#pragma once
#ifndef P3D_ECS_ENTITYSYSTEM_H
#define P3D_ECS_ENTITYSYSTEM_H


#include "entity_manager.h"

class EntitySystem {
public:
    EntitySystem(EntityManager* manager);

    virtual void process(float dt) = 0;
  
protected:
    EntityManager* _manager;
};

#endif