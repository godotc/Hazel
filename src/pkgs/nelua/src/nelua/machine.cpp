//
/*
 * @ Author: godot42
 * @ Create Time: 2025-01-03 00:39:48
 * @ Modified by: @godot42
 * @ Modified time: 2025-01-06 22:47:21
 * @ Description:
 */

#include "machine.h"

#include <cstdarg>
#include <cstdio>



LuaMachine::LuaMachine()
{
    L = luaL_newstate();
    luaL_openlibs(L);
}
LuaMachine::~LuaMachine()
{
    lua_close(L);
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

void StackDump(lua_State *L)
{
    printf("\n***** begin stack dump **\n");
    int n = lua_gettop(L);
    for (int i = 1; i <= n; ++i) {
        int type = lua_type(L, i);
        switch (type) {
        case LUA_TSTRING:
            printf("[%s]", lua_tostring(L, i));
            break;
        case LUA_TBOOLEAN:
            printf("[%s]", lua_toboolean(L, i) ? "true" : "false");
            break;
        case LUA_TNUMBER:
            printf("[%g]", lua_tonumber(L, i));
            break;
        case LUA_TFUNCTION:
            printf("[%s]", lua_tostring(L, i));
            break;
        default:
            printf("[%s]", lua_typename(L, type));
            break;
        }
        printf(" ");
    }

    printf("\n");
    printf("***** end stack dump **\n\n");
}
