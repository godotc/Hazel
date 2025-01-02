
#include <cstdio>
#include <cstdlib>
#include <tuple>
#include <type_traits>
#include <typeinfo>
#include <utility>

extern "C" {
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
}


void StackDump(lua_State *L)
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

void LoadLuaScriptFile(lua_State *L, const char *filename)
{
    luaL_loadfile(L, filename);
    int ret = lua_pcall(L, 0, 0, 0);
    if (ret != 0)
    {
        printf("%d %s\n", ret, lua_tostring(L, -1));
        lua_pop(L, 1);
    }
    else {
        printf("loaded %s\n", filename);
    }
}

bool GetGlobalFunc(lua_State *L, const char *func)
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


bool CallLuaFuncImpl(lua_State *L, int narg, int nret)
{
    int ret = lua_pcall(L, narg, nret, 0);
    if (ret != 0)
    {
        StackDump(L);
        printf("lua_pcall failed %d\n", ret);
        printf("%s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
        return false;
    }
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


template <TLuaPushable T>
void LuaPushValue(lua_State *L, T arg)
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
void LuaPushValues(lua_State *L, T t)
{
    LuaPushValue(L, std::forward<T>(t));
}

template <TLuaPushable T, TLuaPushable... Ts>
void LuaPushValues(lua_State *L, T &&t, Ts &&...ts)
{
    LuaPushValue(L, t);
    LuaPushValues(L, std::forward<Ts>(ts)...);
}


template <TLuaPushable... Args>
bool CallLuaFunc(lua_State *L, const char *func, Args... args)
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
bool CallLuaFuncWithRet(lua_State *L, T &out, const char *func, Args... args)
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



int main()
{
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    luaL_loadstring(L, "print('hello world')");
    int ret = lua_pcall(L, 0, 0, 0);
    if (ret != 0)
    {
        printf("%d %s\n", ret, lua_tostring(L, -1));
        lua_pop(L, 1);
    }

    lua_getglobal(L, "print");
    lua_pushstring(L, "hello world");
    ret = lua_pcall(L, 1, 0, 0);
    if (ret != 0) {
        printf("%d %s\n", ret, lua_tostring(L, -1));
        lua_pop(L, 1);
    }


    CallLuaFunc(L, "print", "hello world");



    struct Pos {
        int x;
        int y;
    } pos = {1, 2};


    LoadLuaScriptFile(L, "C:/Users/norm/1/Hazel/src/pkgs/nelua/scripts/test.lua");

    CallLuaFunc(L, "Add", 1, 2);

    if (int result = -1; CallLuaFuncWithRet(L, result, "Add", 1, 2)) {
        printf("result: %d\n", result);
    }



    lua_close(L);
}
