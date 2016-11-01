#pragma once
#ifndef P3D_ENTITY_MANAGER_H
#define P3D_ENTITY_MANAGER_H

#include "config_module.h"

#include "component.h"
#include "entity.h"

#include <unordered_map>
#include <vector>

class EntityManager
{
    class EntityPool
    {
		friend class EntityManager;
      public:
        using entity_map_t = std::unordered_map<Entity::id_t, Entity *>;

        entity_map_t::iterator begin() { return _entities.begin(); };
        entity_map_t::iterator end() { return _entities.end(); };

      private:
        void register_entity(Entity *ptr);

        entity_map_t _entities;
    };

  public:

    Entity* new_entity();

    void register_entity_with_component(Component::id_t id, Entity *ptr);

    template <typename T>
    inline EntityPool &get_by_component()
    {
        return get_component_pool(T::component_id);
    };

  private:
    inline EntityPool &get_component_pool(Component::id_t id);

    std::unordered_map<Component::id_t, EntityPool> _entities_by_component;
    std::vector<Entity*> _new_entities;
};

#include "entity_manager.I"

#endif