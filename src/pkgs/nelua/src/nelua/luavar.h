//
/*
 * @ Author: godot42
 * @ Create Time: 2025-01-06 20:30:02
 * @ Modified by: @godot42
 * @ Modified time: 2025-01-11 04:56:22
 * @ Description:
 */



#pragma once

#include "lua.h"
#include "types.h"
#include <string>
#include <string_view>
#include <variant>

#include "manager.h"

#include "utility/string_utils.h"



struct LuaVar {

    struct LuaRef {

        int ref;
        int state_idx;
        LuaRef(lua_State *L)
        {
            ref       = luaL_ref(L, LUA_REGISTRYINDEX);
            state_idx = LuaMachineManager::Get().GetIndex(L);
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



    static LuaVar GetValue(lua_State *L, std::string_view path)
    {
        LuaVar ret;

        // push global table
        lua_pushglobaltable(L);
        std::string_view left, right;

        while (ut::str::split(path, '.', left, right)) {
            if (lua_type(L, -1) != LUA_TTABLE) {
                break;
            }
            lua_pushstring(L, left.data());
            lua_gettable(L, -2);

            ret.type = decltype(ret.type)(lua_type(L, -1));

            switch (ret.type) {
            case LUA_TNIL:
            {
                ret.type = ELuaType::Nil;
                break;
            }
            case LUA_TSTRING:
            {
                // TODO: string ref?
                size_t len;
                ret.value.emplace<std::string>(lua_tolstring(L, -1, &len), len);
                ret.type = ELuaType::String;
                break;
            }
            case LUA_TFUNCTION:
            case LUA_TTABLE:
            case LUA_TUSERDATA:
            {
                // Get pos of -2
                lua_gettop(L);

                ret.value.emplace<LuaRef>(L);
                ret.type = ELuaType::UserData;
            }
            case LUA_TLIGHTUSERDATA:
            {
                ret.value = lua_touserdata(L, -1);
                ret.type  = ELuaType::LightUserData;
                break;
            }
            case LUA_TBOOLEAN:
            {
                ret.value = lua_toboolean(L, -1);
                ret.type  = ELuaType::Boolean;
                break;
            }
            case LUA_TNUMBER:
            {
                if (lua_isinteger(L, -1)) {
                    ret.value = lua_tointeger(L, -1);
                    ret.type  = ELuaType::Integer;
                }
                else {
                    ret.value = lua_tonumber(L, -1);
                    ret.type  = ELuaType::Number;
                }
                break;
            }
            default:
                ret.type = ELuaType::Nil;
                break;
            }

            lua_remove(L, -2);
            if (ret.type == ELuaType::Nil) {
                break;
            }
            path = right;
        }
        lua_pop(L, 1);
        return ret;
    }
};