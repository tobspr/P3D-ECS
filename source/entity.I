

inline Entity::Entity(EntityManager* manager) 
    : _manager(manager)
{
    // TODO: Add check if out of bounds
    _id = ++next_id;
    _component_mask = 0u;
}
