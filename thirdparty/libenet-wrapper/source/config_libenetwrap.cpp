
#include "config_libenetwrap.h"

#include "dconfig.h"

#include "enet/enet.h"

Configure(config_libenetwrap);

ConfigureFn(config_libenetwrap) {
  init_libenetwrap();
}

void
init_libenetwrap() {
  static bool initialized = false;
  if (initialized) {
    return;
  }
  initialized = true;

  if (enet_initialize() != 0) {
    fprintf(stderr, "An error occurred while initializing ENet.\n");
  } else {
    std::cout << "Successfully initialized LibENET" << std::endl;
  }
  atexit(enet_deinitialize);

  return;
};
