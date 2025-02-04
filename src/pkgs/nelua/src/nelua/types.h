//
/*
 * @ Author: godot42
 * @ Create Time: 2025-01-06 20:27:55
 * @ Modified by: @godot42
 * @ Modified time: 2025-01-19 06:28:49
 * @ Description:
 */

#pragma once

#include "microshit.h"


extern "C" {
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
}


#include <optional>


extern void StackDump(lua_State *L);


namespace ELuaType {

enum T
{
    Nil = LUA_TNIL,

    Boolean = LUA_TBOOLEAN,

    LightUserData = LUA_TLIGHTUSERDATA,

    Number  = LUA_TNUMBER,
    Integer = -LUA_TNUMBER - 1,

    String = LUA_TSTRING,

    Table    = LUA_TTABLE,
    Function = LUA_TFUNCTION,
    UserData = LUA_TUSERDATA,
    Ref      = -LUA_TUSERDATA - 1,

    Thread = LUA_TTHREAD,

    Tuple,
};
}; // namespace ELuaType



template <typename Arg>
concept TLuaPushable = requires(lua_State *L, Arg arg) {
    {
        lua_pushnumber(L, arg)
    } -> std::convertible_to<void>;
} || requires(lua_State *L, Arg arg) {
    {
        lua_pushstring(L, arg)
    } -> std::convertible_to<const char *>;
} || requires(lua_State *L, Arg arg) {
    {
        lua_pushboolean(L, arg)
    } -> std::convertible_to<void>;
};


struct NELUA_API LuaMachine;