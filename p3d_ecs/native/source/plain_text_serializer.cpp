
#include "plain_text_serializer.h"

#include "entity.h"

namespace serialization_helpers {

	void write_entity(stream_t stream, const entity_ptr_t entity) {
		if (entity == nullptr)
			stream << "-";
		else
			stream << entity->get_uuid().c_str();
	}

}
