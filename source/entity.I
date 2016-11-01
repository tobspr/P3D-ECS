

inline Entity::Entity(EntityManager* manager) : 
    _manager(manager),
    _component_mask(0u)
{
    // TODO: Add check if out of bounds
    _id = ++next_id;
}
