//
/*
 * @ Author: godot42
 * @ Create Time: 2025-01-03 00:29:21
 * @ Modified by: @godot42
 * @ Modified time: 2025-03-04 02:03:44
 * @ Description:
 */


#pragma once
extern "C" {
#include "lua.h"
}
#include "types.h"

#include "luavar.h"
#include "microshit.h"

#include "nelua/manager.h"



#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include <typeinfo>

#include "log.h"


#include "utility/string_utils.h"

class NELUA_API LuaMachine
{
    friend class LuaMachineManager;

    lua_State *L;
    int        index;

  public:
    bool bDebugOuput = false;

  public:

    LuaMachine(lua_State *L, int index);
    virtual ~LuaMachine() {}

    lua_State *GetState() { return L; }
    int        GetIndex() { return index; }
    bool       IsValid() { return L != nullptr && index > 0; }


    void stack_dump(std::string_view sig = "")
    {
        if (!sig.empty()) {
            printf("%s\n", sig.data());
        }
        if (bDebugOuput) {
            StackDump(L);
        }
    }


  public:
    bool LoadFromString(const std::string &str);
    bool LoadLuaScriptFile(const char *filename, int nret = 0);


    void RegisterGlobalFunc(const char *name, lua_CFunction func)
    {
        lua_register(L, name, func);
        // if (lua_error(L)) {
        //     fprintf(stderr, "lua_register failed: %s\n", lua_tostring(L, -1));
        //     lua_pop(L, 1);
        // }
        if (lua_getglobal(L, name) != LUA_TFUNCTION) {
            Log("lua_register failed: %s\n", lua_tostring(L, -1));
        }
        lua_pop(L, 1);
    }


    bool CallMemberFunc(const char *path, const char *member_func)
    {
        Log("-->>CallLuaMemberFunc %s.%s", path, member_func);
        LuaVar var = LuaVar::GetValue(L, path);
        if (var.type == ELuaType::Nil) {
            Log("failed to get %s\n", path);
            return false;
        }

        if (var.type != ELuaType::Table) {
            Log("failed to get %s, not a table: %d\n", path, var.type);
        }
        bool ret = false;

        lua_pushstring(L, member_func);
        lua_gettable(L, -2);
        if (lua_isfunction(L, -1)) {
            ret = call_luafunc_impl(L, 0, 0);
        }
        Log("--<< end CallLuaMemberFunc %s.%s\n", path, member_func);

        StackDump(L);
        return ret;
    }

    /**
     * @param bPureFunc true: call Table.Func(...) , false: call Table:Func(...)
     */
    bool CallMemberFuncV2(const char *path, const char *member_func, bool bPureFunc)
    {
        Log("-->>begin CallLuaMemberFuncV2 %s.%s", path, member_func);

        std::vector<std::string> result;
        result = ut::str::split(path, '.');

        bool bGetFunc = false;

        if (result.size() == 1) {
            lua_getglobal(L, path);
            // lua_pushvalue(L, -1);
            lua_pushstring(L, member_func);
            lua_gettable(L, -2);

            if (lua_isfunction(L, -1)) {
                bGetFunc = true;
            }
        }
        else {
            // get global table
            lua_getglobal(L, result[0].c_str());
            // lua_pushvalue(L, -1);

            // iterate to last table
            for (size_t i = 1; i < result.size(); ++i) {
                lua_pushstring(L, result[i].c_str());
                lua_gettable(L, -2);
                Log("lua_gettable %s", result[i].c_str());
                // lua_pushvalue(L, -1);
            }

            // get function
            lua_pushstring(L, member_func);
            lua_gettable(L, -2);
            if (lua_isfunction(L, -1)) {
                bGetFunc = true;
            }
        }

        bool ret = false;

        stack_dump();
        if (bGetFunc) {
            if (bPureFunc) {
                ret = call_luafunc_impl(L, 0, 0);
            }
            else {
                lua_pushvalue(L, -2);
                ret = call_luafunc_impl(L, 1, 0);
            }
        }
        else {
            Log("failed to get %s.%s\n", path, member_func);
        }

        Log("--<<end CallLuaMemberFuncV2 %s.%s\n", path, member_func);
        lua_pop(L, result.size());

        StackDump(L);

        return ret;
    }

    bool LuaCall(int nargs, int nret)
    {
        return call_luafunc_impl(L, nargs, nret);
    }


    template <TLuaPushable T>
    void PushValue(T v)
    {
        push_value(std::forward<T>(v));
    }


    template <TLuaPushable... Args>
    bool CallFunc(const char *func, Args... args)
    {
        Log("\n-->>CallLuaFunc %s\n", func);

        if (!get_global_func(func)) {
            return false;
        }
        push_values(std::forward<Args>(args)...);

        stack_dump();

        if (!call_luafunc_impl(L, sizeof...(args), 0)) {
            return false;
        }

        Log("--<<end pcall %s\n", func);
        return true;
    }

    bool CallFunc(const char *func)
    {
        Log("\n-->>CallLuaFunc %s\n", func);

        if (!get_global_func(func)) {
            return false;
        }

        stack_dump();

        if (!call_luafunc_impl(L, 0, 0)) {
            return false;
        }

        Log("--<<end pcall %s\n", func);
        return true;
    }



    template <typename T, TLuaPushable... Args>
    bool CallFuncWithRet(T &out, const char *func, Args... args)
    {
        Log("\n-->>CallLuaFuncWithRet %s\n", func);

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
        Log("lua_getglobal %s, Type: %s\n", func, lua_typename(L, -1));
        if (type != LUA_TFUNCTION) {
            Log("failed to get global %s\n", func);
            Log("%d %s\n", type, lua_tostring(L, -1));
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
        Log("Unknown type %s\n", typeid(arg).name());
        exit(-1);
    }


    template <TLuaPushable T>
    int push_values(T t)
    {
        push_value(std::forward<T>(t));
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
