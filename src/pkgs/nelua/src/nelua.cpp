
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


template <typename... Ts>
struct type_list {
};


template <typename, size_t>
struct nth;

// end recursive condition
template <typename T, typename... Remains>
struct nth<type_list<T, Remains...>, 0> {
    using type = T;
};

template <typename T, typename... Remains, size_t N>
struct nth<type_list<T, Remains...>, N> {
    using type = typename nth<type_list<Remains...>, N - 1>::type;
};


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

    if constexpr (std::is_floating_point_v<Arg> || std::is_integral_v<Arg>)
    {
        lua_pushnumber(L, arg);
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

template <TLuaPushable... Ts>
void LuaPushValues(lua_State *L, Ts &&...ts)
{
    using types = std::tuple<Ts...>;

    // nth<types, Is>::type arg = std::get<Is>(std::forward<types>(ts))>
    // (LuaPushValue(L, std::get<Is>(std::forward_as_tuple(ts...)))...);
}

template <TLuaPushable... Args>
void CallLuaFunc(lua_State *L, const char *func, Args... args)
{
    printf("pcall %s\n", func);

    if (int type = lua_getglobal(L, func); type != LUA_TFUNCTION) {
        printf("%d %s\n", type, lua_tostring(L, -1));
        lua_pop(L, 1);
        return;
    }

    (LuaPushValues(L, args)...);

    int ret = lua_pcall(L, sizeof...(args), 0, 0);
    if (ret != 0)
    {
        printf("%s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
    }
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

    luaL_loadfile(L, "C:/Users/norm/1/Hazel/src/pkgs/nelua/scripts/test.lua");



    CallLuaFunc(L, "print", "hello world");



    struct Pos {
        int x;
        int y;
    } pos = {1, 2};

    CallLuaFunc(L, "add", 1, 2);

    // CallLuaFunc(L, "print", pos);



    lua_close(L);
}