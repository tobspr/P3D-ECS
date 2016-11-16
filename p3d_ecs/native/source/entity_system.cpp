
#include "entity_system.h"
#include "leak_detector.h"

EntitySystem::EntitySystem(EntityManager* manager) {
  ECS_ON_CREATE("EntitySystem");
  _manager = manager;
}

EntitySystem::~EntitySystem() {
  ECS_ON_DELETE("EntitySystem");
}
