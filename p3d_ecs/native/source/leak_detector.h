#pragma once

#ifndef P3D_ECS_LEAK_DETECTOR_H
#define P3D_ECS_LEAK_DETECTOR_H

#include "config_module.h"
#include <string>

namespace p3d_ecs {
  namespace leak_detector {
    void on_construct(const std::string& cls_name);
    void on_destruct(const std::string& cls_name);
    void print_leaks();
    void reset_leaks();
    size_t get_num_leaks();
  }
}

// Convenience macros
#if !defined(INTERROGATE) && !defined(PROFILING)
#define ECS_ON_CREATE(class_name) ::p3d_ecs::leak_detector::on_construct(class_name);
#define ECS_ON_DELETE(class_name) ::p3d_ecs::leak_detector::on_destruct(class_name);
#else
#define ECS_ON_CREATE(class_name)
#define ECS_ON_DELETE(class_name)
#endif

#endif