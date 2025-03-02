/**
 * @ Author: godot42
 * @ Create Time: 2024-11-13 17:47:25
 * @ Modified by: @godot42
 * @ Modified by: @godot42
 * @ Modified time: 2025-03-03 02:39:46
 */


#include "component.h"
#include "hazel/scene/scriptable_entity.h"
#include "hazel/script/script_engine.h"
#include "nelua/types.h"
#include "scene.h"



namespace hazel {


void NativeScriptComponent::DestroyScriptFuncImpl(NativeScriptComponent *nsc)
{
    delete nsc->Instance;
    nsc->Instance = nullptr;
}


void CameraComponent::OnComponentAddedImpl(const Scene *scene)
{
    auto w = scene->GetViewportWidth();
    auto h = scene->GetViewportHeight();
    if (w > 0 && h > 0) {
        Camera.SetViewportSize(w, h);
    }
}

void LuaScriptComponent::Init()
{
    auto LM    = ScriptEngine::GetMachine();
    ScriptPath = "C:/Users/norm/1/Hazel/example/project0/script.lua";
    if (!LM.LoadLuaScriptFile(ScriptPath.c_str(), 1)) {
        HZ_CORE_ERROR("Failed to load script: {}", ScriptPath);
    }
    LM.stack_dump();
    // lua_pushstring(LM.GetState(), "New");
    // lua_gettable(LM.GetState(), -2);
    lua_getfield(LM.GetState(), -1, "New");
    if (lua_type(LM.GetState(), -1) != LUA_TFUNCTION) {
        HZ_CORE_ERROR("new is not a function {}", ScriptPath);
    }
    lua_pushvalue(LM.GetState(), -2);
    if (!LM.LuaCall(1, 1)) {
        HZ_CORE_ERROR("Failed to call New of {}", ScriptPath);
    }
    if (lua_type(LM.GetState(), -1) != LUA_TTABLE) {
        HZ_CORE_ERROR("not table after new {}", ScriptPath);
    }

    HZ_INFO("created script obj {}", ScriptPath);
    LM.stack_dump("after new");

    lua_pushvalue(LM.GetState(), -1);
    refScriptObj = luaL_ref(LM.GetState(), LUA_REGISTRYINDEX); // -1 stack
    LM.stack_dump("after ref 1");

    // lua_pushvalue(LM.GetState(), -1);
    int ret = lua_getfield(LM.GetState(), -1, "OnCreate");
    if (ret != LUA_TFUNCTION) {
        HZ_CORE_ERROR("Failed to load script: {}", ScriptPath);
    }
    refOnCreate = luaL_ref(LM.GetState(), LUA_REGISTRYINDEX); // -1 stack

    // lua_pushvalue(LM.GetState(), -1);
    ret = lua_getfield(LM.GetState(), -1, "OnUpdate");
    if (ret != LUA_TFUNCTION) {
        HZ_CORE_ERROR("Failed to load script: {}", ScriptPath);
    }
    refOnUpdate = luaL_ref(LM.GetState(), LUA_REGISTRYINDEX); // -1 stack

    // lua_pushvalue(LM.GetState(), -1);
    ret = lua_getfield(LM.GetState(), -1, "OnDestroy");
    if (ret != LUA_TFUNCTION) {
        HZ_CORE_ERROR("Failed to load script: {}", ScriptPath);
    }
    refOnDestroy = luaL_ref(LM.GetState(), LUA_REGISTRYINDEX);

    HZ_INFO("refOnCreate {} refOnUpdate {} refOnDestroy {}", refOnCreate, refOnUpdate, refOnDestroy);

    LM.stack_dump("after ref all functions");
    lua_pop(LM.GetState(), 2);
    bInitialized = true;
    LM.stack_dump("after init script");

    this->OnCreate();
}

void LuaScriptComponent::OnCreate()
{
    if (refOnCreate != LUA_NOREF) {
        auto LM = ScriptEngine::GetMachine();
        lua_rawgeti(LM.GetState(), LUA_REGISTRYINDEX, refOnCreate);
        lua_rawgeti(LM.GetState(), LUA_REGISTRYINDEX, refScriptObj);
        LM.LuaCall(0, 0);
    }
}

void LuaScriptComponent::OnUpdate(float ts)
{
    if (refOnUpdate != LUA_NOREF) {
        auto LM = ScriptEngine::GetMachine();
        lua_rawgeti(LM.GetState(), LUA_REGISTRYINDEX, refOnUpdate);
        lua_rawgeti(LM.GetState(), LUA_REGISTRYINDEX, refScriptObj);
        // LM.stack_dump();
        LM.PushValue(ts);
        LM.LuaCall(2, 0);
    }
}

void LuaScriptComponent::OnDestroy()
{
    if (refOnDestroy != LUA_NOREF) {
        auto LM = ScriptEngine::GetMachine();
        lua_rawgeti(LM.GetState(), LUA_REGISTRYINDEX, refScriptObj);
        lua_rawgeti(LM.GetState(), LUA_REGISTRYINDEX, refOnDestroy);
        LM.LuaCall(0, 0);
    }

    luaL_unref(ScriptEngine::GetMachine().GetState(), LUA_REGISTRYINDEX, refScriptObj);
    luaL_unref(ScriptEngine::GetMachine().GetState(), LUA_REGISTRYINDEX, refOnCreate);
    luaL_unref(ScriptEngine::GetMachine().GetState(), LUA_REGISTRYINDEX, refOnUpdate);
    luaL_unref(ScriptEngine::GetMachine().GetState(), LUA_REGISTRYINDEX, refOnDestroy);
}

} // namespace hazel