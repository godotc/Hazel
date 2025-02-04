//
/*
 * @ Author: godot42
 * @ Create Time: 2025-01-06 20:30:02
 * @ Modified by: @godot42
 * @ Modified time: 2025-01-19 06:33:56
 * @ Description:
 */



#pragma once

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


struct LuaVar {

    struct LuaRef {

        int ref;
        int state_idx;
        int pos;
        LuaRef(lua_State *L)
        {
            // ref       = luaL_ref(L, LUA_REGISTRYINDEX);
            // state_idx = LuaMachineManager::Get().GetIndex(L);
            // pos       = lua_gettop(L);
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
        type = (ELuaType::T)lua_type(L, pos);
        log("set value type of %d : %d -> %s", pos, type, lua_typename(L, type));

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
            value.emplace<LuaRef>(L);
            break;
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