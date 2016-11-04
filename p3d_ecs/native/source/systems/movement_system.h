#pragma once
#ifndef P3D_ECS_MOVEMENTSYSTEM_H
#define P3D_ECS_MOVEMENTSYSTEM_H

#include "entity_system.h"

class MovementSystem : public EntitySystem {
public:
  MovementSystem(EntityManager *manager);

  virtual void process(float dt) override;

private:
  EntityCollector *_relevant_entities;
};

#endif