

#pragma once



#include "lua.h"
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
    int32_t    GetIndex(lua_State *L);
};