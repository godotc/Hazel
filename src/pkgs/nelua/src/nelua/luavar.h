//
/*
 * @ Author: godot42
 * @ Create Time: 2025-01-06 20:30:02
 * @ Modified by: @godot42
 * @ Modified time: 2025-02-07 01:29:21
 * @ Description:
 */



#pragma once

#include "lauxlib.h"
extern "C" {
#include "lua.h"
}
#include "types.h"

#include <cstdio>
#include <string>
#include <string_view>
#include <variant>

#include "manager.h"

#include "utility/string_utils.h"

#include "log.h"


struct LuaRef {

    static int create_ref(lua_State *L, bool bWeak)
    {
        lua_newtable(L); // new_table={}

        if (bWeak) {
            lua_newtable(L); // metatable={}

            lua_pushliteral(L, "__mode");
            lua_pushliteral(L, "v");
            // n's -2 = -1 -> new_table's "__mode" = "v"
            lua_rawset(L, -3); // metatable._mode='v'

            lua_setmetatable(L, -2); // setmetatable(new_table,metatable)
        }

        lua_pushvalue(L, -2);  // push the previous top of stack
        lua_rawseti(L, -2, 1); // new_table[1]=original value on top of the stack

        // Now new_table is on top of the stack, rest is up to you
        // Here is how you would store the reference:
        return luaL_ref(L, LUA_REGISTRYINDEX); // this pops the new_table
    }
};


struct LuaVar {


    std::variant<lua_Integer,
                 lua_Number,
                 std::string,
                 void *, // userdata/table
                 bool>
                value;
    ELuaType::T type;

    LuaVar() { type = ELuaType::Nil; }


    void SetValue(lua_State *L, int pos)
    {
        type = (ELuaType::T)lua_type(L, pos);
        log("set value type of pos %d : %d -> %s", pos, type, lua_typename(L, type));

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
        {
            type = ELuaType::Function;
        }

        case LUA_TTABLE:
        {
            type = ELuaType::Table;
        }
        case LUA_TUSERDATA:
        {
            type = ELuaType::UserData;
            break;
        }
        case LUA_TLIGHTUSERDATA:
        {
            type  = ELuaType::LightUserData;
            value = lua_touserdata(L, -1);
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

    LuaVar Get(lua_State *L, std::string_view key)
    {
        log("index %s, cur luavar type: %d", key.data(), type);
        if (type == ELuaType::Table) {
            log("get table %s", key.data());
            lua_pushstring(L, key.data());
            int w = lua_gettable(L, -2);
            log("w %d", w);
        }
        else if (type == ELuaType::Nil) {
            log("get nil, get global %s", key.data());
            lua_getglobal(L, key.data());
        }

        SetValue(L, -1);
        // lua_pushvalue(L, std::get<LuaRef>(ret.value).pos);

        return *this;
    }


    static LuaVar GetValue(lua_State *L, std::string_view path)
    {
        LuaVar ret;

        // push global table
        // lua_pushglobaltable(L);
        std::string      left;
        std::string_view right;

        bool bFound = false;

        do {
            bFound = ut::str::split(path, '.', left, right);
            log("left %s, right %s", left.data(), right.data());
            ret = ret.Get(L, left);
            StackDump(L);
            log("LuaVar::Get ret type: %d", ret.type);
            if (ret.type != ELuaType::Table) {
                break;
            }
            if (right.empty()) {
                break;
            }
            path = right;
        } while (bFound);

        // lua_pop(L, 1);
        return ret;
    }
};