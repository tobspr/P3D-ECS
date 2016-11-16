
#include "config_module.h"

#include "dconfig.h"
#include "uuid.h"

Configure(config_mymodule);

ConfigureFn(config_mymodule) {
  init_libmymodule();
}

void
init_libmymodule() {
  static bool initialized = false;
  if (initialized) {
    return;
  }
  initialized = true;
  UUID::init_uuid_rng();
  return;
};
