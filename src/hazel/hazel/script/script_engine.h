//
/*
 * @ Author: godot42
 * @ Create Time: 2025-01-02 23:02:52
 * @ Modified by: @godot42
 * @ Modified time: 2025-03-02 15:57:32
 * @ Description:
 */


#pragma once


#include <hazel/core/base.h>

#include "nelua/machine.h"
#include "nelua/types.h"


namespace hazel {


// this will be a game instance subsystem?
class ScriptEngine
{

    static LuaMachine LM;

  public:
    static void PreInit();
    static void Init();
    static void Shutdown();


    static LuaMachine &GetMachine() { return LM; }
};

}; // namespace hazel