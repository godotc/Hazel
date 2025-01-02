
#pragma once


#include <cstdio>
#include <cstdlib>
#include <string>
#include <tuple>
#include <type_traits>
#include <typeinfo>
#include <utility>

extern "C" {
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
}


inline void StackDump(lua_State *L)
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



class LuaMachine
{
    lua_State *L;

    LuaMachine()
    {
        L = luaL_newstate();
        luaL_openlibs(L);
    }

    ~LuaMachine()
    {
        lua_close(L);
    }


  public:
    bool LoadString(const std::string &str)
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


    bool LoadLuaScriptFile(const char *filename)
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

    template <TLuaPushable... Args>
    bool PCall(const char *func, Args... args)
    {
        printf("\n-->>CallLuaFunc %s\n", func);

        if (!GetGlobalFunc(L, func)) {
            return false;
        }
        LuaPushValues(L, std::forward<Args>(args)...);

        StackDump(L);

        if (!CallLuaFuncImpl(L, sizeof...(args), 0)) {
            return false;
        }

        printf("--<<end pcall %s\n", func);
        return true;
    }



    template <typename T, TLuaPushable... Args>
    bool PCallWithRet(lua_State *L, T &out, const char *func, Args... args)
    {
        printf("\n-->>CallLuaFuncWithRet %s\n", func);

        if (!GetGlobalFunc(L, func)) {
            return false;
        }

        LuaPushValues(L, std::forward<Args>(args)...);

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
    void push_values(T t)
    {
        push_value(L, std::forward<T>(t));
    }

    template <TLuaPushable T, TLuaPushable... Ts>
    void push_values(T &&t, Ts &&...ts)
    {
        push_value(L, t);
        push_values(L, std::forward<Ts>(ts)...);
    }
};
