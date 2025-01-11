//
/*
 * @ Author: godot42
 * @ Create Time: 2025-01-06 20:33:36
 * @ Modified by: @godot42
 * @ Modified time: 2025-01-11 05:27:45
 * @ Description:
 */



#pragma once

#include "microshit.h"
#include "types.h"
#include <atomic>
#include <cstdint>
#include <map>


struct NELUA_API LuaMachineManager {

    std::map<lua_State *, int32_t> L2Idx;
    std::map<int32_t, lua_State *> Idx2L;
    std::atomic<int32_t>           Index = 0;


    static LuaMachineManager &Get()
    {
        static LuaMachineManager manager;
        return manager;
    }

    LuaMachine NewMachine();
    bool       RemoveMachine(lua_State *L);
    bool       RemoveMachine(LuaMachine &machine);
    int32_t    GetIndex(lua_State *L);
};