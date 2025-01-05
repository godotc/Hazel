//
/*
 * @ Author: godot42
 * @ Create Time: 2025-01-06 20:30:02
 * @ Modified by: @godot42
 * @ Modified time: 2025-01-06 23:01:44
 * @ Description:
 */



#pragma once

#include "machine.h"
#include "types.h"
#include <variant>



struct LuaRef {

    int ref;
    int state_idx;
    LuaRef(lua_State *L)
    {
        ref = luaL_ref(L, LUA_REGISTRYINDEX);
        // state_idx = LuaMachineManager::GetLMByLuaState(L)->index;
    }
};

struct LuaVar {
    // using value_type = std::variant<
    //     lua_Integer,
    //     lua_Number,
    //     std::string,
    //     void *,
    //     bool>;

    // struct interal_value_tyep {
    //     value_type  value;
    //     ELuaType::T type;
    // };

    // value_type *values;
    // size_t      count;

    // void SetFromLua(lua_State *L, int pos, ELuaType::T type)
    // {
    // }

    std::variant<lua_Integer,
                 lua_Number,
                 std::string,
                 void *, // userdata/table
                 bool>
                value;
    ELuaType::T type;



    template <typename T>
    static LuaVar GetValue(lua_State *L, int pos, ELuaType::T type)
    {
        LuaVar ret;

        // push global table
        lua_pushglobaltable(L);

        std::string_view path(key);
        std::string_view left, right;

        LuaVar ret = {};

        while (string_split(path, '.', left, right)) {
            if (lua_type(L, -1) != LUA_TTABLE)
                break;
            lua_pushstring(L, left.data());
            lua_gettable(L, -2);


            // LuaVar::Type type = LV_NIL;
            ret.type = lua_type(L, -1);
            switch (ret.type) {
            case LUA_TNIL:
            {
                ret.type = ELuaType::Nil;
                break;
            }
            case LUA_TSTRING:
            {
                size_t      len;
                const char *buf = lua_tolstring(L, -1, &len);
                ret.value       = std::string(buf, len);
                ret.type        = ELuaType::String;
                break;
            }
            case LUA_TFUNCTION:
            case LUA_TTABLE:
            case LUA_TUSERDATA:
            {
                LuaRef ref(L);
                ret.type = ELuaType::UserData;
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
            case LUA_TLIGHTUSERDATA:
            {
                ret.value = lua_touserdata(L, -1);
                ret.type  = ELuaType::LightUserData;
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