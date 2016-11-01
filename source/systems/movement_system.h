#pragma once
#ifndef P3D_ECS_MOVEMENTSYSTEM_H
#define P3D_ECS_MOVEMENTSYSTEM_H


#include "entity_system.h"
#include "entity_manager.h"

class MovementSystem : public EntitySystem {
public:
    MovementSystem(EntityManager* manager) :
        EntitySystem(manager),
        _relevant_entities(manager->new_collector<TransformComponent, PhysicsComponent>())
    {
    };

    virtual void process(float dt) override;

private:
    EntityCollector _relevant_entities;
};

#endif