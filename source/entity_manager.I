

EntityManager::EntityPool& EntityManager::get_component_pool(Component::id_t id)
{
    auto it = _entities_by_component.find(id);
    if (it == _entities_by_component.end())
    {
        _entities_by_component.emplace(id, EntityPool {});
        it = _entities_by_component.find(id); // xxx: find better solution
    }
    return it->second;
};
