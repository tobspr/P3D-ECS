#pragma once

#ifndef P3D_ECS_PERFUTILTIY_H
#define P3D_ECS_PERFUTILTIY_H

#include <vector>
#include <algorithm>

template <typename T>
void vector_erase_fast(std::vector<T>& vec, T elem)
{
    // Fast removal by swapping with the last element and resizing by -1
    // Requires: vec contains elem
    assert(!vec.empty());
    auto it = std::find(vec.begin(), vec.end(), elem);
    assert(it != vec.end());
    vector_erase_fast(vec, it);
}

template <typename T>
void vector_erase_fast(std::vector<T>& vec, typename std::vector<T>::iterator it)
{
	assert(!vec.empty());
    auto last_element_iter = std::prev(vec.end());
    if (it == last_element_iter) {
        // We are removing the last element, just reduce size by 1
        vec.pop_back();
    } else {
        // We are removing any arbitrary element, swap with last element and pop last
        std::swap(it, last_element_iter);
        vec.pop_back();
    }
}

#endif