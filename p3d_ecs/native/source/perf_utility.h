#pragma once

#ifndef P3D_ECS_PERFUTILTIY_H
#define P3D_ECS_PERFUTILTIY_H

#include "config_module.h"
#include <vector>
#include <algorithm>

namespace p3d_ecs {
  namespace performance {

    template <typename T>
    ECS_FORCEINLINE typename ::std::vector<T>::const_iterator vector_find(
      const ::std::vector<T>& vec, const T& elem) {
      return ::std::find(vec.cbegin(), vec.cend(), elem);
    }

    template <typename T>
    ECS_FORCEINLINE typename ::std::vector<T>::iterator vector_find(::std::vector<T>& vec,
                                                                    const T& elem) {
      return ::std::find(vec.begin(), vec.end(), elem);
    }

    template <typename T>
    ECS_FORCEINLINE bool vector_contains(const ::std::vector<T>& vec, const T& elem) {
      return ::std::find(vec.cbegin(), vec.cend(), elem) != vec.cend();
    }

    template <typename T>
    void vector_erase(::std::vector<T>& vec, typename ::std::vector<T>::iterator it) {
      ECS_ASSERT(!vec.empty());
      auto last_element_iter = ::std::prev(vec.end());
      if (it == last_element_iter) {
        // We are removing the last element, just reduce size by 1
        vec.pop_back();
      } else {
        // We are removing any arbitrary element, swap with last element and pop
        // last
        ::std::swap(*it, *last_element_iter);
        vec.pop_back();
      }
    }

    template <typename T>
    bool vector_erase_if_contains(::std::vector<T>& vec, const T& elem) {
      // Fast removal by swapping with the last element and resizing by -1
      if (vec.empty())
        return false;
      auto it = vector_find(vec, elem);
      if (it != vec.end()) {
        vector_erase(vec, it);
        return true;
      }
      return false;
    }

    template <typename T>
    void vector_erase(::std::vector<T>& vec, const T& elem) {
      // Fast removal by swapping with the last element and resizing by -1
      // Requires: vec contains elem
      ECS_ASSERT(!vec.empty());
      auto it = vector_find(vec, elem);
      ECS_ASSERT(it != vec.end());
      vector_erase(vec, it);
    }

    template <typename T>
    bool compare_flat_sets(const ::std::vector<T>& a, const ::std::vector<T>& b) {
      if (a.size() != b.size())
        return false;

      // O(n^2), use with care
      for (const T& elem : a) {
        if (!vector_contains(b, elem))
          return false;
      }

      return true;
    }
  }

  namespace utility {

    ECS_FORCEINLINE bool float_equals(float a, float b) { return ::std::abs(a - b) < 1e-10; }
  }
}

#endif