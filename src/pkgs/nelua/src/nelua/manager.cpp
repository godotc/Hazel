#include "lua.h"
#include "types.h"

#include "machine.h"
#include "manager.h"
#include <cstdint>



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
    if (auto it = L2Idx.find(L); it != L2Idx.end()) {
        lua_State *L = it->first;
        Idx2L.erase(it->second);
        L2Idx.erase(it->first);
        lua_close(L);
        return true;
    }
    return false;
}

int32_t LuaMachineManager::GetIndex(lua_State *L)
{
    if (auto it = L2Idx.find(L); it != L2Idx.end()) {
        return it->second;
    }
    return -1;
}
