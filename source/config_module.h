#pragma once

#include "pandabase.h"
#include "notifyCategoryProxy.h"
#include "configVariableDouble.h"
#include "configVariableString.h"
#include "configVariableInt.h"

NotifyCategoryDecl(mymodule, EXPORT_CLASS, EXPORT_TEMPL);

extern void init_libmymodule();


#if 1
#define WANT_LEAK_DETECTION 1
#include <map>
#include <string>
#include <iostream>

void alloc_instance(const std::string& msg);
void dealloc_instance(const std::string& msg);
void print_memory_leaks();

#define ECS_OUTPUT_DEBUG(content) std::cout << content << std::endl;
#define ECS_ON_CREATE(class_name) alloc_instance(class_name);
#define ECS_ON_DELETE(class_name) dealloc_instance(class_name);
#define ECS_PRINT_LEAKS() print_memory_leaks();
#else
#define ECS_OUTPUT_DEBUG(content)
#define ECS_ON_CREATE(content)
#define ECS_ON_DELETE(content)
#define ECS_PRINT_LEAKS()
#endif