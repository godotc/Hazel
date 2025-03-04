//
/*
 * @ Author: godot42
 * @ Create Time: 2025-01-02 23:02:52
 * @ Modified by: @godot42
 * @ Modified time: 2025-03-05 01:50:17
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

    std::vector<luaL_Reg> m_Functions;

  public:
    static void Test();

    static void Init();
    static void Shutdown();


    static int NativeLog(lua_State *L);

    void LogLua(std::string msg);


    static LuaMachine &GetMachine() { return LM; }
};

}; // namespace hazel