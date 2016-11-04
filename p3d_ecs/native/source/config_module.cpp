
#include "config_module.h"

/*

Include all your dynamically typed classes here, e.g.
#include "my_dynamic_class.h"

*/

#include "dconfig.h"

Configure(config_mymodule);
NotifyCategoryDef(mymodule, "");

ConfigureFn(config_mymodule) { init_libmymodule(); }

void init_libmymodule() {
  static bool initialized = false;
  if (initialized) {
    return;
  }
  initialized = true;

  // Init your dynamic types here, e.g.:
  // MyDynamicClass::init_type();

  return;
}

#if WANT_LEAK_DETECTION

std::map<std::string, int> _MEMORY_LEAKS_TRACKING;

void alloc_instance(const std::string &msg) {
  auto it = _MEMORY_LEAKS_TRACKING.find(msg);
  if (it == _MEMORY_LEAKS_TRACKING.end()) {
    _MEMORY_LEAKS_TRACKING[msg] = 1;
    // std::cout << "After alloc, there are now 1 instances of " << msg <<
    // std::endl;
  } else {
    ++(it->second);
    // std::cout << "After alloc, there are now " << it->second << " instances
    // of "<< msg << std::endl;
  }
}

void dealloc_instance(const std::string &msg) {
  _MEMORY_LEAKS_TRACKING[msg]--;
  // std::cout << "After dealloc, there are now " << _MEMORY_LEAKS_TRACKING[msg]
  // << " instances of " << msg << std::endl;
}

void print_memory_leaks() {
  bool any_leak = false;
  for (auto it : _MEMORY_LEAKS_TRACKING) {
    if (it.second != 0) {
      std::cout << "LEAK: There are " << it.second << " instances of "
                << it.first << " left!" << std::endl;
      any_leak = true;
    }
  }
  if (!any_leak) {
    std::cout << "No leaks detected." << std::endl;
  }
}

void reset_memory_leaks() { _MEMORY_LEAKS_TRACKING.clear(); }

#endif