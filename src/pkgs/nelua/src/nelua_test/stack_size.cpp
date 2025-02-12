

#include "nelua/types.h"
#include "gtest/gtest.h"

#include "nelua/luavar.h"


TEST(nelua, create_ref_with_stack_size)
{
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    lua_newtable(L);
    int n = lua_gettop(L);
    log("stack size: %d", n);

    LuaRef::create_ref(L, true);

    n = lua_gettop(L);
    log("stack size: %d", n);
}