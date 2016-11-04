#pragma once

#ifndef P3D_ECS_${CLASSNAME_UPPER}_H
#define P3D_ECS_${CLASSNAME_UPPER}_H

#include "${META_HEADER}"
#include "perf_utility.h"

class ${CLASSNAME} final : public ${CLASSNAME}Meta {
public:
  DEFINE_COMPONENT(${CLASSNAME});

  // Add your custom getters/setters here

private:

  inline ${CLASSNAME}(Entity *entity) : superclass(entity){};

  // Add your custom additional attributes here, notice they
  // are not serialized
};
#endif
