
#include "output.h"
#include "perf_utility.h"
#include "termcolor.h"
#include <iostream>

namespace p3d_ecs {

  std::vector<Warning> MessageManager::_warnings;

  void MessageManager::warn(Warning nr, const std::string& title, const std::string& message) {
    std::cerr << termcolor::yellow << termcolor::bold << "  WARNING " << title << ": ";
    std::cerr << message << termcolor::reset << std::endl;
    _warnings.push_back(nr);
  }

  const std::vector<Warning>& MessageManager::get_occurred_warnings() { return _warnings; }
  void MessageManager::clear_occured_warnings() { _warnings.clear(); }

  std::vector<Warning> MessageManager::take_occurred_warnings() {
    std::vector<Warning> result;
    _warnings.swap(result);
    return std::move(result);
  }

  bool MessageManager::take_warning(Warning nr) {
    return ::p3d_ecs::performance::vector_erase_if_contains(_warnings, nr);
  }

  bool MessageManager::take_warnings(const std::vector<Warning>& nrs) {
    for (Warning w : nrs) {
      if (!take_warning(w)) {
        return false;
      }
    }
    return true;
  }
}
