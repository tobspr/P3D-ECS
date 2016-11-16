#pragma once
#ifndef P3D_ECS_CONFIG_MODULE_H
#define P3D_ECS_CONFIG_MODULE_H

#include "pandabase.h"

extern void init_libmymodule();

#ifdef _MSC_VER
#ifndef INTERROGATE
#define ECS_FORCEINLINE __forceinline
#endif
#endif

#ifndef ECS_FORCEINLINE
#define ECS_FORCEINLINE inline
#endif

// For maximum performance
#define PROFILING

#ifdef PROFILING
#define ECS_ASSERT(condition)
#else
#define ECS_ASSERT(condition) assert(condition)
#endif

#endif
