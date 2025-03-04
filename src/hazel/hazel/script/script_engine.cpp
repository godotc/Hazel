//
/*
 * @ Author: godot42
 * @ Create Time: 2025-01-02 23:03:51
 * @ Modified by: @godot42
 * @ Modified time: 2025-03-05 01:52:14
 * @ Description:
 */


#include "hazel/core/app.h"
#include "hz_pch.h"


#include "script_engine.h"

#include "nelua/machine.h"
#include <filesystem>


#include "nelua/manager.h"
#include "utils/file.h"
#include "utils/path.h"
#include <utf8/cpp20.h>


// #include <nethost.h>



namespace hazel {


LuaMachine ScriptEngine::LM(nullptr, -1);

namespace fs = std::filesystem;

void ScriptEngine::Test()
{
    // only for test before renderer setup
    LM             = LuaMachineManager::Get().NewMachine();
    LM.bDebugOuput = true;

    LM.stack_dump("preinit");

    // need preload some lua codes?
    fs::path main    = FPath("src/pkgs/nelua/scripts/main.lua");
    auto     Content = utils::File::read_all(main);

    HZ_CORE_ASSERT(Content.has_value(), "Failed to read main script");
    LM.LoadFromString(Content.value());

    LM.CallFunc("print_hello");

    // LM.CallMemberFunc("Module.Guide", "print_hello");
    LM.CallMemberFuncV2("Module.Guide", "print_hello", false);

    LuaMachineManager::Get().RemoveMachine(LM);

    // App::Get().Shutdown();
}

void ScriptEngine::Init()
{
    LM             = LuaMachineManager::Get().NewMachine();
    LM.bDebugOuput = true;
    lua_State *L   = LM.GetState();

    luaL_openlibs(L);


    LM.RegisterGlobalFunc("NativeLog", &ScriptEngine::NativeLog);

    {
        LM.stack_dump("before update package.path");

        lua_getglobal(L, "package");
        LM.stack_dump("after get package");
        lua_getfield(L, -1, "path");
        std::string path = lua_tostring(L, -1);


        path = (App::Get().GetWorkingDirectory() / "?.lua").string() + ";" + path;
        lua_pushstring(L, path.c_str());

        LM.stack_dump("on update package.path");

        lua_setfield(L, -3, "path");

        lua_pushvalue(L, -2);
        lua_setglobal(L, "package");

        LM.stack_dump("after update package.path");
        lua_pop(L, 2);
    }
}

void ScriptEngine::Shutdown()
{
    LuaMachineManager::Get().RemoveMachine(LM);
}

int ScriptEngine::NativeLog(lua_State *L)
{
    // TODO: arg1 should be a log level
    int         n = lua_gettop(L);
    std::string msg;
    msg.reserve(n * 4);
    for (int i = 1; i <= n; i++) {
        const char *s = lua_tostring(L, i);
        msg += s;
        msg.push_back(' ');
    }
    HZ_INFO("LogLua {}", msg);
    return 0;
}

void ScriptEngine::LogLua(std::string msg)
{
    // lua_pushstring(LM.GetState(), msg.c_str());
    LM.CallFunc("print", msg.c_str());
}



} // namespace hazel