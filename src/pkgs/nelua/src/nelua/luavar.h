//
/*
 * @ Author: godot42
 * @ Create Time: 2025-01-06 20:30:02
 * @ Modified by: @godot42
 * @ Modified time: 2025-01-11 06:12:28
 * @ Description:
 */



#pragma once

#include "lua.h"
#include "types.h"

#include <cstdio>
#include <string>
#include <string_view>
#include <variant>

#include "manager.h"

#include "utility/string_utils.h"



struct LuaVar {

    struct LuaRef {

        int ref;
        int state_idx;
        int pos;
        LuaRef(lua_State *L)
        {
            ref       = luaL_ref(L, LUA_REGISTRYINDEX);
            state_idx = LuaMachineManager::Get().GetIndex(L);
            pos       = lua_gettop(L);
        }
    };

    std::variant<lua_Integer,
                 lua_Number,
                 std::string,
                 void *, // userdata/table
                 LuaRef,
                 bool>
                value;
    ELuaType::T type;

    LuaVar() { type = ELuaType::Nil; }


    void SetValue(lua_State *L, int pos)
    {
        int type = lua_type(L, pos);

        switch (type) {
        case LUA_TNIL:
        {
            type = ELuaType::Nil;
            break;
        }
        case LUA_TSTRING:
        {
            // TODO: string ref?
            size_t len;
            value.emplace<std::string>(lua_tolstring(L, -1, &len), len);
            type = ELuaType::String;
            break;
        }
        case LUA_TFUNCTION:
        case LUA_TTABLE:
        case LUA_TUSERDATA:
        {
            value.emplace<LuaRef>(L);
            type = ELuaType::UserData;
        }
        case LUA_TLIGHTUSERDATA:
        {
            value = lua_touserdata(L, -1);
            type  = ELuaType::LightUserData;
            break;
        }
        case LUA_TBOOLEAN:
        {
            value = lua_toboolean(L, -1);
            type  = ELuaType::Boolean;
            break;
        }
        case LUA_TNUMBER:
        {
            if (lua_isinteger(L, -1)) {
                value = lua_tointeger(L, -1);
                type  = ELuaType::Integer;
            }
            else {
                value = lua_tonumber(L, -1);
                type  = ELuaType::Number;
            }
            break;
        }
        default:
            type = ELuaType::Nil;
            break;
        }


        return;
    }

    static LuaVar Get(lua_State *L, std::string_view key)
    {
        LuaVar ret;

        printf("index %s\n", key.data());
        if (lua_type(L, -1) != LUA_TTABLE) {
            printf("not a table\n");
        }
        lua_pushstring(L, key.data());
        lua_getglobal(L, key.data());

        ret.SetValue(L, -1);

        return ret;
    }


    static LuaVar GetValue(lua_State *L, std::string_view path)
    {
        LuaVar ret;

        // push global table
        lua_pushglobaltable(L);
        std::string_view left, right;

        while (ut::str::split(path, '.', left, right)) {
            ret = LuaVar::Get(L, left);
            if (ret.type == ELuaType::Nil) {
                break;
            }

            path = right;
        }
        lua_pop(L, 1);
        return ret;
    }
};