#pragma once

#ifndef P3D_ECS_ENTITYCOLLECTOR_H
#define P3D_ECS_ENTITYCOLLECTOR_H

#include "component.h"
#include "perf_utility.h"

#include <vector>
#include <unordered_set>
#include <initializer_list>
#include <memory>

class Entity;

class EntityCollector final {
    /**
   * The entity collector is a class which collects all entities which
   * have certain components. For example:
   *    EntityCollector::create<TransformComponent, PhysicsComponent>()
   * will create a collector which gives access to all entities with at least
   * a transform and physics component.
   */

    using component_list_t = std::vector<Component::id_t>;
    using entity_list_t = std::vector<Entity*>;

    EntityCollector(const component_list_t& components);

public:
    struct EntityIterator {
        
		// Iterator to iterate over an entity vector, and check for entity
        // validty while iterating
        using iter_t = entity_list_t::iterator;

        EntityIterator(iter_t pos, iter_t end, Component::bitmask_t target_mask);

        EntityIterator& operator++();
		inline Entity* operator*() { return *_pos; };
        inline bool operator==(const EntityIterator& other) const { return _pos == other._pos; }
        inline bool operator!=(const EntityIterator& other) const { return _pos != other._pos; }
        inline bool operator< (const EntityIterator& other) const { return _pos <  other._pos; }
        inline bool operator> (const EntityIterator& other) const { return _pos >  other._pos; }
        
    private:
        bool is_valid_entity(Entity* entity) const;
        void inc_until_valid();

        iter_t _pos, _end;
        Component::bitmask_t _target_mask;
    };

    template <typename... Components>
    static EntityCollector* create()
    {
        component_list_t components = { Component::extract_id<Components>()... };
        return new EntityCollector(components);
    };

    ~EntityCollector()
    {
        ECS_ON_DELETE("EntityCollector");
    }

    void consider_register(Entity* entity);
    void consider_remove_entity(Entity* entity);

    bool entity_fits(Entity* entity) const;

    void remove_entity(Entity* entity);

    inline const component_list_t& get_components() const { return _components; };
    inline Component::bitmask_t get_component_mask() const
    {
        return _component_mask;
    };

    inline EntityIterator begin()
    {
		return EntityIterator { _matching_entities.begin(), _matching_entities.end(), _component_mask };
    };

    inline EntityIterator end()
    {
		return EntityIterator { _matching_entities.end(), _matching_entities.end(), _component_mask };
    };

    inline size_t size() const { return _matching_entities.size(); };

    void print_status();

private:
    component_list_t _components;
    entity_list_t _matching_entities;
    Component::bitmask_t _component_mask;
};

#endif