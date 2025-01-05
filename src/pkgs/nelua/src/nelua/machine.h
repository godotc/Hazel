//
/*
 * @ Author: godot42
 * @ Create Time: 2025-01-03 00:29:21
 * @ Modified by: @godot42
 * @ Modified time: 2025-01-06 22:58:59
 * @ Description:
 */


#pragma once

#include "microshit.h"

#include "types.h"


#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <typeinfo>



extern void StackDump(lua_State *L);


class NELUA_API LuaMachine
{
    lua_State *L;
    int        index;

    bool bDebugOuput;

  public:
    LuaMachine();
    virtual ~LuaMachine();

    void log(const char *fmt, ...)
    {
        if (bDebugOuput) {
            va_list args;
            va_start(args, fmt);
            vfprintf(stderr, fmt, args);
            va_end(args);
        }
    }

    void stack_dump()
    {
        if (bDebugOuput) {
            StackDump(L);
        }
    }


  public:
    bool LoadFromString(const std::string &str);
    bool LoadLuaScriptFile(const char *filename);



    bool CallMemberFunc(const char *path, const char *member_func)
    {
        return false;
    }


    template <TLuaPushable... Args>
    bool CallFunc(const char *func, Args... args)
    {
        log("\n-->>CallLuaFunc %s\n", func);

        if (!get_global_func(func)) {
            return false;
        }
        push_values(L, std::forward<Args>(args)...);

        stack_dump();

        if (!call_luafunc_impl(L, sizeof...(args), 0)) {
            return false;
        }

        log("--<<end pcall %s\n", func);
        return true;
    }

    bool CallFunc(const char *func)
    {
        log("\n-->>CallLuaFunc %s\n", func);

        if (!get_global_func(func)) {
            return false;
        }

        stack_dump();

        if (!call_luafunc_impl(L, 0, 0)) {
            return false;
        }

        log("--<<end pcall %s\n", func);
        return true;
    }



    template <typename T, TLuaPushable... Args>
    bool CallFuncWithRet(T &out, const char *func, Args... args)
    {
        log("\n-->>CallLuaFuncWithRet %s\n", func);

        if (!get_global_func(func)) {
            return false;
        }

        push_values(L, std::forward<Args>(args)...);

        if (!call_luafunc_impl(L, sizeof...(args), 1)) {
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


    bool get_global_func(const char *func)
    {
        int type = lua_getglobal(L, func);
        log("lua_getglobal %s, Type: %s\n", func, lua_typename(L, -1));
        if (type != LUA_TFUNCTION) {
            log("failed to get global %s\n", func);
            log("%d %s\n", type, lua_tostring(L, -1));
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
        log("Unknown type %s\n", typeid(arg).name());
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

    bool call_luafunc_impl(lua_State *L, int nargs, int nret);
};
