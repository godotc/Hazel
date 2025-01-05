

#pragma once



#include "microshit.h"
#include "types.h"
#include <map>
#include <vector>

struct NELUA_API LuaMachineManager {

    static LuaMachine *MainLM;

    static std::vector<LuaMachine>    LMs;
    static std::map<lua_State *, int> luastate_2_index;


    LuaMachine *GetMainLM()
    {
        return nullptr;
    }
};