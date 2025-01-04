//
/*
 * @ Author: godot42
 * @ Create Time: 2025-01-03 00:29:21
 * @ Modified by: @godot42
 * @ Modified time: 2025-01-05 01:51:31
 * @ Description:
 */


#pragma once

#include "microshit.h"

#include "nelua/machine.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <string_view>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>



extern "C" {
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
}


extern void StackDump(lua_State *L);

namespace ELuaType {
using T = int; // Type
enum
{
    None = LUA_TNONE,
    Nil  = LUA_TNIL,

    Boolean = LUA_TBOOLEAN,

    LightUserData = LUA_TLIGHTUSERDATA,

    Number  = LUA_TNUMBER,
    Integer = -LUA_TNUMBER - 1,

    String   = LUA_TSTRING,
    Table    = LUA_TTABLE,
    Function = LUA_TFUNCTION,
    UserData = LUA_TUSERDATA,
    Thread   = LUA_TTHREAD,

    Tuple,
};
}; // namespace ELuaType



inline std::string string_replace(const std::string &source, const std::string &from, const std::string &to)
{
    std::string ret = source;
    size_t      pos = 0;
    while ((pos = ret.find(from, pos)) != std::string::npos) {
        ret.replace(pos, from.size(), to);
        pos += to.size();
    }
    return ret;
}


struct SplitStringResult {
    std::string left;
    std::string right;
};

inline std::vector<std::string> string_split(std::string_view source, char delimiter = ' ')
{
    std::vector<std::string> ret;
    int                      nth = 0;
    // "abc def"
    while ((nth = source.find(delimiter)) != std::string::npos) {
        ret.emplace_back(source.substr(0, nth - 1));
        source.remove_prefix(nth + 1);
    }
    return ret;
}


inline bool string_split(std::string_view source, char sep, std::string_view &left, std::string_view &right)
{
    int n = source.find_first_of(sep);
    if (n == std::string::npos) {
        return false;
    }
    left  = source.substr(0, n);
    right = source.substr(n + 1, source.size() - n - 1);
    return true;
}


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



class NELUA_API LuaMachine
{
    lua_State *L;

  public:
    LuaMachine();
    virtual ~LuaMachine();


  public:
    bool LoadFromString(const std::string &str);
    bool LoadLuaScriptFile(const char *filename);


    template <typename T>
    T GetValue(lua_State *L, int pos, ELuaType::T type)
    {
        switch (type) {
        case ELuaType::Nil:
            break;
        case ELuaType::Integer:
            return lua_tointeger(L, pos);
            break;
        case ELuaType::Number:
            return lua_tonumber(L, pos);
            break;
        case ELuaType::String:
        {
            size_t      len;
            const char *buf = lua_tolstring(L, pos, &len);
            std::string s(buf, len);
            return s;
        }
        case ELuaType::Boolean:
            return !!lua_toboolean(L, pos);
        case ELuaType::LightUserData:
            struct LuaLightUserData {
                void       *ptr;
                ELuaType::T type;
            };
            return LuaLightUserData{
                .ptr  = lua_touserdata(L, pos),
                .type = type,

            };
        case ELuaType::Function:
        case ELuaType::Table:
        case ELuaType::UserData:
            // alloc(1);
            // lua_pushvalue(L, pos);
            // vars[0].ref     = new RefRef(l);
            // vars[0].luatype = type;
            // break;
        case ELuaType::Tuple:

            struct LuaTuple {
                std::vector<ELuaType::T> types;
                std::vector<void *>      values;
            };
            if (!(pos > 0 && lua_gettop(L) >= pos)) {
                printf("wtf lua_gettop(L) %d", lua_gettop(L));
                exit(-1);
            }

            // initTuple(l, p);
            break;
        default:
            break;
        }
    }

    template <typename T>
    T Get(const char *key)
    {
        // push global table
        lua_pushglobaltable(L);

        std::string_view path(key);
        std::string_view left, right;
        T                rt;

        while (string_split(path, '.', left, right)) {
            if (lua_type(L, -1) != LUA_TTABLE)
                break;
            lua_pushstring(L, left.data());
            lua_gettable(L, -2);
            rt.set(L, -1);

            ELuaType::T type = lua_type(L, -1);
            // LuaVar::Type type = LV_NIL;
            switch (type) {
            case LUA_TSTRING:
            case LUA_TFUNCTION:
            case LUA_TTABLE:
            case LUA_TUSERDATA:
            case LUA_TBOOLEAN:
                break;
            case LUA_TNUMBER:
            {
                if (lua_isinteger(L, -1)) {
                    lua_tointeger(L, -1);
                    type = ELuaType::Integer;
                }
                else {
                    lua_tonumber(L, -1);
                    type = ELuaType::Number;
                }
            } break;
            case LUA_TLIGHTUSERDATA:
                type = ELuaType::LightUserData;
                break;
            case LUA_TNIL:
            default:
                type = ELuaType::Nil;
                break;
            }

            lua_remove(L, -2);
            if (rt.isNil())
                break;

            path = right;
        }
        lua_pop(L, 1);
        return rt;
    }

    template <TLuaPushable... Args>
    bool PCall(const char *func, Args... args)
    {
        printf("\n-->>CallLuaFunc %s\n", func);

        if (!GetGlobalFunc(func)) {
            return false;
        }
        push_values(L, std::forward<Args>(args)...);

        StackDump(L);

        if (!CallLuaFuncImpl(L, sizeof...(args), 0)) {
            return false;
        }

        printf("--<<end pcall %s\n", func);
        return true;
    }



    template <typename T, TLuaPushable... Args>
    bool PCallWithRet(T &out, const char *func, Args... args)
    {
        printf("\n-->>CallLuaFuncWithRet %s\n", func);

        if (!GetGlobalFunc(func)) {
            return false;
        }

        push_values(L, std::forward<Args>(args)...);

        if (!CallLuaFuncImpl(L, sizeof...(args), 1)) {
            return false;
        }

        using t = std::remove_cv_t<std::remove_reference_t<T>>;
        if constexpr (std::is_same_v<t, float>) {
            out = lua_tonumber(L, -1);
            return true;
        }
        else if constexpr (std::is_same_v<t, int>) {
            out = lua_tointeger(L, -1);
            return true;
        }

        return false;
    }


  private:


    bool GetGlobalFunc(const char *func)
    {
        int type = lua_getglobal(L, func);
        printf("lua_getglobal %s, Type: %s\n", func, lua_typename(L, -1));
        if (type != LUA_TFUNCTION) {
            printf("failed to get global %s\n", func);
            printf("%d %s\n", type, lua_tostring(L, -1));
            lua_pop(L, 1);
            return false;
        }
        return true;
    }

    template <TLuaPushable T>
    void push_value(T arg)
    {
        using Arg = std::remove_cv_t<std::remove_reference_t<T>>;

        if constexpr (std::is_floating_point_v<Arg>)
        {
            // now use float, maybe double in lua
            lua_pushnumber(L, float(arg));
            return;
        }
        if constexpr (std::is_integral_v<Arg>)
        {
            // now use int, maybe long long in lua
            lua_pushnumber(L, int(arg));
            return;
        }
        if constexpr (std::is_same_v<Arg, const char *>)
        {
            lua_pushstring(L, arg);
            return;
        }
        if constexpr (std::is_same_v<Arg, bool>)
        {
            lua_pushboolean(L, arg);
            return;
        }
        if constexpr (std::is_same_v<Arg, nullptr_t>)
        {
            lua_pushnil(L);
            return;
        }
        // Add more types as needed
        fprintf(stderr, "Unknown type %s\n", typeid(arg).name());
        exit(-1);
    }


    template <TLuaPushable T>
    int push_values(T t)
    {
        push_value(L, std::forward<T>(t));
        return 1;
    }

    template <TLuaPushable T, TLuaPushable... Ts>
    int push_values(T &&t, Ts &&...ts)
    {
        push_value(L, t);
        return 1 + push_values(L, std::forward<Ts>(ts)...);
    }
};
