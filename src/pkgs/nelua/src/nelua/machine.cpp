//
/*
 * @ Author: godot42
 * @ Create Time: 2025-01-03 00:39:48
 * @ Modified by: @godot42
 * @ Modified time: 2025-02-07 01:18:21
 * @ Description:
 */

#include "machine.h"

#include <cstdarg>
#include <cstdio>



LuaMachine::LuaMachine(lua_State *L, int index)
{
    this->L     = L;
    this->index = index;
}

bool LuaMachine::LoadFromString(const std::string &str)
{
    luaL_loadstring(L, str.c_str());
    int ret = lua_pcall(L, 0, 0, 0);
    if (ret != 0)
    {
        printf("%d %s\n", ret, lua_tostring(L, -1));
        lua_pop(L, 1);
        return false;
    }

    return true;
}

bool LuaMachine::LoadLuaScriptFile(const char *filename)
{
    luaL_loadfile(L, filename);
    int ret = lua_pcall(L, 0, 0, 0);
    if (ret != 0)
    {
        printf("%d %s\n", ret, lua_tostring(L, -1));
        lua_pop(L, 1);
        return false;
    }

    printf("loaded %s\n", filename);
    return true;
}

bool LuaMachine::call_luafunc_impl(lua_State *L, int nargs, int nret)
{
    int ret = lua_pcall(L, nargs, nret, 0);
    if (ret != LUA_OK) {
        log("lua_pcall failed: %s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
        return false;
    }
    return true;
}
