
#include <gtest/gtest.h>

#include<filesystem>

#include "nelua/log.h"
#include "nelua/types.h"



TEST(nelua, basic_usage)
{
    lua_State *L = luaL_newstate();

    luaL_openlibs(L);
    // system("dir");
    // auto cur_dir = std::filesystem::current_path();
    // log("cur dir: %s", cur_dir.string().c_str());
    ASSERT_TRUE(std::filesystem::exists("./basic.lua"));
    luaL_loadfile(L, "./basic.lua");
    int n = lua_gettop(L);
    log("stack size: %d", n);

    int r = lua_pcall(L, 0, 0, 0);
    log("r %d", r);
}
