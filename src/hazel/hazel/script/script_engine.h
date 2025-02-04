//
/*
 * @ Author: godot42
 * @ Create Time: 2025-01-02 23:02:52
 * @ Modified by: @godot42
 * @ Modified time: 2025-01-19 05:40:54
 * @ Description:
 */


#pragma once


#include <hazel/core/base.h>

#include "nelua/machine.h"
#include "nelua/types.h"


namespace hazel {


class ScriptEngine
{

    static LuaMachine LM;

  public:
    static void PreInit();
    static void Init();
    static void Shutdown();
};

}; // namespace hazel