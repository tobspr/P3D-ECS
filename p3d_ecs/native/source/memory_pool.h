#pragma once

#ifndef P3D_ECS_MEMORYPOOL_H
#define P3D_ECS_MEMORYPOOL_H

#include "config_module.h"
#include <memory>
#include <iostream>
#include <type_traits>
#include <cassert>
#include <vector>

#ifndef PROFILING
#define DETECT_BAD_ALLOC
#endif

template <typename T>
class MemoryPool {
public:
  static const size_t obj_size = sizeof(T);
  static const size_t block_size = 524288; // 512 KB

  template <typename... Args>
  inline static T* new_object(Args... args) {
#ifdef DETECT_BAD_ALLOC
    ++_size;
#endif
    T* mem = alloc_memory();
    ::new ((void*)mem) T(std::forward<Args>(args)...);
    return mem;
  }

  inline static T* new_pod_object() {
#ifdef DETECT_BAD_ALLOC
    ++_size;
#endif
    return alloc_memory();
  }

  inline static void delete_object(T* ptr) {
    if (ptr) {
#ifdef DETECT_BAD_ALLOC
      ECS_ASSERT(_size > 0);
      --_size;
#endif
      _free_objects.push_back(ptr);
      ptr->~T();
#ifdef DETECT_BAD_ALLOC
      memset(ptr, 0x00, sizeof(T));
#endif
    }
  }

  inline static void delete_pod_object(T* ptr) {
    if (ptr) {
#ifdef DETECT_BAD_ALLOC
      ECS_ASSERT(_size > 0);
      --_size;
#endif
      _free_objects.push_back(ptr);
#ifdef DETECT_BAD_ALLOC
      memset(ptr, 0x00, sizeof(T));
#endif
    }
  }

  template <typename Up>
  inline static void delete_object_from_upcast(Up* ptr) {
    if (ptr) {
#ifdef DETECT_BAD_ALLOC
      assert(_size > 0);
      --_size;
#endif
      _free_objects.push_back(reinterpret_cast<T*>(ptr));
#ifdef DETECT_BAD_ALLOC
      memset(ptr, 0x00, sizeof(T));
#endif
    }
  }

  inline static void reset() {
    for (char* block : _blocks) {
      free(block);
    }
    _blocks.clear();
    _free_objects.clear();
    _last_index = 0;
  }

  inline static size_t get_object_count() {
#ifdef DETECT_BAD_ALLOC
    return _size;
#else
    return 0;
#endif
  }

private:
  inline static T* alloc_memory() {
    if (!_free_objects.empty()) {
      T* mem = reinterpret_cast<T*>(_free_objects.back());
      _free_objects.pop_back();
      return mem;
    }
    // Find new spot in block
    size_t block_offs = _last_index % block_size;
    size_t block_id = _last_index / block_size;
    ++_last_index;
    if (block_id >= _blocks.size()) {
      // Alloc new block
      char* mem = static_cast<char*>(malloc(block_size * obj_size));
      _blocks.push_back(mem);
      _free_objects.reserve(_blocks.size() * block_size);
      return reinterpret_cast<T*>(mem);
    } else {
      return reinterpret_cast<T*>(_blocks[block_id] + obj_size * block_offs);
    }
  }

  static size_t _last_index;

#ifdef DETECT_BAD_ALLOC
  static size_t _size;
#endif

  static std::vector<char*> _blocks;
  static std::vector<T*> _free_objects;
};

template <typename T>
size_t MemoryPool<T>::_last_index = 0;

#ifdef DETECT_BAD_ALLOC
template <typename T>
size_t MemoryPool<T>::_size = 0;
#endif

template <typename T>
std::vector<char*> MemoryPool<T>::_blocks;
template <typename T>
std::vector<T*> MemoryPool<T>::_free_objects;

#endif