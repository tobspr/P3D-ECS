#pragma once

#include "pandabase.h"
#include "notifyCategoryProxy.h"
#include "configVariableDouble.h"
#include "configVariableString.h"
#include "configVariableInt.h"


NotifyCategoryDecl(mymodule, EXPORT_CLASS, EXPORT_TEMPL);

extern void init_libmymodule();

#if 1
    #include <iostream>
    #define ECS_OUTPUT_DEBUG(content) std::cout << content << std::endl;

#else
    #define ECS_OUTPUT_DEBUG(content)

#endif