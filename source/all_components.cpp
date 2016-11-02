
#include "all_components.h"
#include "entity.h"

IMPLEMENT_COMPONENT(TransformComponent, 1u);
IMPLEMENT_COMPONENT(PhysicsComponent, 2u);

void TransformComponent::set_parent(Entity* new_parent)
{
    ECS_OUTPUT_DEBUG("Setting parent to " << *new_parent);
    assert(new_parent == nullptr || new_parent->has_component<TransformComponent>());

    _dirty = true;
    // Deregister from old parent
    if (_parent != nullptr)
        _parent->get_component<TransformComponent>().unregister_child(_entity);

    // Register to new parent
    _parent = new_parent;
    if (_parent != nullptr)
        _parent->get_component<TransformComponent>().register_child(_entity);
}

void TransformComponent::unregister_from_parent()
{
    if (_parent)
        _parent->get_component<TransformComponent>().unregister_child(_entity);
};
