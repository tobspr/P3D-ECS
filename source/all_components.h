#pragma once
#ifndef P3D_ECS_ALLCOMPONENTS_H
#define P3D_ECS_ALLCOMPONENTS_H

#include "component.h"

#include "luse.h"

// All available components

struct TransformComponent : public Component
{
    DEFINE_COMPONENT();

    LVecBase3f pos;
    LVecBase3f scale;
    LVecBase3f hpr;

    inline TransformComponent(const LVecBase3f &pos, const LVecBase3f &scale, const LVecBase3f &hpr)
        : pos(pos), scale(scale), hpr(hpr){};
};

struct PhysicsComponent : public Component
{
    DEFINE_COMPONENT();

    float mass;
    LVecBase2f velocity;

    inline PhysicsComponent(float mass, const LVecBase2f &velocity)
        : velocity(velocity), mass(mass){};
};

#endif