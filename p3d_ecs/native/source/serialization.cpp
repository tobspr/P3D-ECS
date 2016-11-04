
#include "serialization.h"

#include "entity.h"

void PlainTextSerializer::write_entity(const entity_ptr_t entity) {
  if (entity == nullptr)
    _out << Entity::EMPTY_ID;
  else
    _out << entity->get_id();
}
