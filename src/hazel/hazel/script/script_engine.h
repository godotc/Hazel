//
/*
 * @ Author: godot42
 * @ Create Time: 2025-01-02 23:02:52
 * @ Modified by: @godot42
 * @ Modified time: 2025-01-11 05:05:07
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
    static void Init();
    static void Shutdown();
};

}; // namespace hazel