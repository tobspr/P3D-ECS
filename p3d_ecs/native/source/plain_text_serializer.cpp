
#include "plain_text_serializer.h"

#include "entity.h"

void
PlainTextSerializer::write_entity(const entity_ptr_t entity) {
  if (entity == nullptr)
    _out << "-";
  else
    _out << entity->get_uuid().c_str();
}
