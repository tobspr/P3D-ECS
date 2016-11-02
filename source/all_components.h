#pragma once
#ifndef P3D_ECS_ALLCOMPONENTS_H
#define P3D_ECS_ALLCOMPONENTS_H

#include "component.h"

#include "luse.h"
#include "perf_utility.h"
#include <vector>

class Entity;
class EntityManager;

struct TransformComponent : public Component {
	friend class EntityManager;
	DEFINE_COMPONENT();

    using child_vec_t = std::vector<Entity*>;

    inline TransformComponent(Entity* entity, const LVecBase3f& pos, const LVecBase3f& scale, const LVecBase3f& hpr)
        : Component(entity)
        , _pos(pos)
        , _scale(scale)
        , _hpr(hpr)
        , _dirty(false)
        , _parent(nullptr){};
    inline TransformComponent(Entity* entity)
        : Component(entity)
        , _dirty(false)
        , _parent(nullptr){};

    inline void set_pos(const LVecBase3f& pos)
    {
        _pos = pos;
        recompute_mat();
    };
    inline void set_scale(const LVecBase3f& scale)
    {
        _scale = scale;
        recompute_mat();
    };
    inline void set_hpr(const LVecBase3f& hpr)
    {
        _hpr = hpr;
        recompute_mat();
    };
    inline void set_mat(const LMatrix4f& mat)
    {
        _mat = mat;
        _dirty = true; /* todo: reconstruct pos and so on? or maybe set a flag like _custom_mat */
    };

    inline const LVecBase3f& get_pos() const { return _pos; }
    inline const LVecBase3f& get_hpr() const { return _hpr; }
    inline const LVecBase3f& get_scale() const { return _scale; }
    inline const LMatrix4f& get_mat() const { return _mat; }

    inline void recompute_mat()
    { /* todo */
        _dirty = true;
    };

    inline bool is_dirty() const { return _dirty; };
    inline Entity* get_parent() const { return _parent; };
    inline const child_vec_t& get_children() const { return _children; };

    void set_parent(Entity* entity);

protected:
    inline void unregister_child(Entity* entity) { vector_erase_fast(_children, entity); };
    inline void register_child(Entity* entity) { /* TODO: assert(!_children.contains(entity)) */ _children.push_back(entity); }

    void unregister_from_parent();

    LVecBase3f _pos;
    LVecBase3f _scale;
    LVecBase3f _hpr;
    LMatrix4f _mat;
    bool _dirty;
    Entity* _parent;
    child_vec_t _children;
};

struct PhysicsComponent : public Component {
    DEFINE_COMPONENT();

    float mass;
    LVecBase2f velocity;

    inline PhysicsComponent(Entity* entity, float mass, const LVecBase2f& velocity)
        : Component(entity)
        , velocity(velocity)
        , mass(mass){};

    inline PhysicsComponent(Entity* entity)
        : Component(entity){};
};

#endif