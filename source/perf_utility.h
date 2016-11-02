#pragma once

#ifndef P3D_ECS_PERFUTILTIY_H
#define P3D_ECS_PERFUTILTIY_H

#include <vector>
#include <algorithm>


template <typename T>
void vector_erase_fast_if_present(std::vector<T>& vec, T elem)
{
    // Fast removal by swapping with the last element and resizing by -1
    if (vec.empty())
        return;
    auto it = std::find(vec.begin(), vec.end(), elem);
    if(it != vec.end())
        vector_erase_fast(vec, it);
}

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
    T tmp = *it;
    ECS_OUTPUT_DEBUG("EF> Removing " << tmp << " from container of size " << vec.size());
	assert(!vec.empty());
    auto last_element_iter = std::prev(vec.end());
    if (it == last_element_iter) {
        // We are removing the last element, just reduce size by 1
        ECS_OUTPUT_DEBUG("EF> Removing last element");
        vec.pop_back();
    } else {
        // We are removing any arbitrary element, swap with last element and pop last
        ECS_OUTPUT_DEBUG("EF> Swapping with last element which is " << *last_element_iter);
        std::swap(*it, *last_element_iter);
        vec.pop_back();
    }
    // just to be sure
    assert(std::find(vec.begin(), vec.end(), tmp) == vec.end());
}

#endif