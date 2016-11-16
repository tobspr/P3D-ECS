
#include "leak_detector.h"

#include <map>
#include <cassert>
#include <iostream>

namespace p3d_ecs {
  namespace leak_detector {
    std::map<std::string, size_t> instance_counts;

    void on_construct(const std::string& cls_name) {
      auto it = instance_counts.find(cls_name);
      if (it == instance_counts.end()) {
        instance_counts[cls_name] = 1;
      } else {
        ++(it->second);
      }
    }

    void on_destruct(const std::string& cls_name) {
      ECS_ASSERT(instance_counts[cls_name] > 0);
      --instance_counts[cls_name];
    }

    void print_leaks() {
      if (get_num_leaks() != 0) {
        for (auto it : instance_counts) {
          if (it.second != 0) {
            std::cout << "LEAK: There are " << it.second << " instances of " << it.first << " left!"
                      << std::endl;
          }
        }
      }
    }

    void reset_leaks() { instance_counts.clear(); }

    size_t get_num_leaks() {
      size_t count = 0;
      for (auto it : instance_counts) {
        count += it.second;
      }
      return count;
    }
  }
}
