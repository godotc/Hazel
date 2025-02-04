//
/*
 * @ Author: godot42
 * @ Create Time: 2025-01-11 04:04:25
 * @ Modified by: @godot42
 * @ Modified time: 2025-01-23 05:33:05
 * @ Description:
 */

#include "types.h"

#include "machine.h"



LuaMachine LuaMachineManager::NewMachine()
{

    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    L2Idx.insert({L, Index});
    Idx2L.insert({Index, L});
    ++Index;

    return LuaMachine(L, Index);
}

bool LuaMachineManager::RemoveMachine(lua_State *L)
{
    if (!L) {
        return false;
    }

    if (auto it = L2Idx.find(L); it != L2Idx.end()) {
        lua_State *L = it->first;
        Idx2L.erase(it->second);
        L2Idx.erase(it->first);
        lua_close(L);
        return true;
    }
    return false;
}

bool LuaMachineManager::RemoveMachine(LuaMachine &machine)
{
    return RemoveMachine(machine.L);
}



int32_t LuaMachineManager::GetIndex(lua_State *L)
{
    if (auto it = L2Idx.find(L); it != L2Idx.end()) {
        return it->second;
    }
    return -1;
}
