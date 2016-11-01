
inline Entity::Entity(EntityManager* manager)
    : _manager(manager)
    , _component_mask(0u)
    , _registered(false)
    , _deleted(false)
{
    ECS_ON_CREATE("Entity");
    // TODO: Add check if out of capacity
    _id = ++next_id;
}
