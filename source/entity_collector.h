#pragma once

#ifndef P3D_ECS_ENTITYCOLLECTOR_H
#define P3D_ECS_ENTITYCOLLECTOR_H

#include "component.h"

#include <vector>
#include <unordered_set>

class EntityCollector
{
    /**
     * The entity collector is a class which collects all entities which
     * have certain components. For example:
     *    EntityCollector::create<TransformComponent, PhysicsComponent>()
     * will create a collector which gives access to all entities with at least
     * a transform and physics component.
     */

    using component_list = std::vector<Component::id_t>;
    using entity_list = std::unordered_set<Entity*>;

    EntityCollector(component_list components);

    template < typename T >
    void unpack_list(component_list &target)
    {
        target.push_back(T::component_id);
    };

#ifndef INTERROGATE
    template < typename T1, typename T2, typename... Args >
    void unpack_list(component_list &target)
    {
        target.push_back(T1::component_id);
        target.push_back(T2::component_id);
        unpack_list<Args...>(target);
    };
#endif

  public:
#ifndef INTERROGATE
    template < typename... Components >
    static EntityCollector *create()
    {
        component_list components;
        unpack_list<Components...>(components);
        return new EntityCollector(components);
    };
#endif

    inline void consider_register(Entity* entity) {
        if ((entity->get_component_mask() & _component_mask) == _component_mask)
        {
            _matching_entities.emplace(entity);
        }
    };

    inline const component_list& get_components() const { return _components; };
    inline Component::bitmask_t get_component_mask() const { return _component_mask; };

    inline entity_list::const_iterator begin() const { return _matching_entities.cbegin(); };
    inline entity_list::const_iterator end() const { return _matching_entities.cend(); };

    inline entity_list::iterator begin() { return _matching_entities.begin(); };
    inline entity_list::iterator end() { return _matching_entities.end(); };

    inline size_t size() const { return _components.size(); };

private:
    component_list _components;
    entity_list _matching_entities;
    Component::bitmask_t _component_mask;
};

#endif