//
/*
 * @ Author: godot42
 * @ Create Time: 2025-01-02 23:03:51
 * @ Modified by: @godot42
 * @ Modified time: 2025-01-23 05:33:24
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


// #include <nethost.h>



namespace hazel {


LuaMachine ScriptEngine::LM(nullptr, -1);

namespace fs = std::filesystem;

void ScriptEngine::PreInit()
{


    LM             = LuaMachineManager::Get().NewMachine();
    LM.bDebugOuput = true;

    fs::path main    = FPath("src/pkgs/nelua/scripts/main.lua");
    auto     Content = utils::File::read_all(main);

    HZ_CORE_ASSERT(Content.has_value(), "Failed to read main script");
    LM.LoadFromString(Content.value());

    LM.CallFunc("print_hello");

    // LM.CallMemberFunc("Module.Guide", "print_hello");
    LM.CallMemberFuncV2("Module.Guide", "print_hello",false);

    App::Get().Shutdown();
}

void ScriptEngine::Init()
{
}

void ScriptEngine::Shutdown()
{
    LuaMachineManager::Get().RemoveMachine(LM);
}



} // namespace hazel