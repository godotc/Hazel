//
/*
 * @ Author: godot42
 * @ Create Time: 2025-01-02 23:03:51
 * @ Modified by: @godot42
 * @ Modified time: 2025-01-06 20:58:04
 * @ Description:
 */


#include "hz_pch.h"


#include "script_engine.h"

#include "nelua/machine.h"
#include <filesystem>


#include "utils/file.h"
#include "utils/path.h"


// #include <nethost.h>



namespace hazel {


LuaMachine *ScriptEngine::LM = nullptr;

namespace fs = std::filesystem;

void ScriptEngine::Init()
{
    LM = new LuaMachine();

    fs::path main    = FPath("src/pkgs/nelua/scripts/main.lua");
    auto     Content = utils::File::read_all(main);

    HZ_CORE_ASSERT(Content.has_value(), "Failed to read main script");
    LM->LoadFromString(Content.value());

    LM->CallFunc("print_hello");
}

void ScriptEngine::Shutdown()
{
    delete LM;
}



} // namespace hazel