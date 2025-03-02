--[[
/**
 *  Author: @godot42
 *  Create Time: 2024-07-21 01:48:13
 *  Modified by: @godot42
 *  Modified time: 2024-07-28 16:01:33
 *  Description:
 */
 ]]



add_rules("mode.debug", "mode.release")
-- add_rules("debug_game")

-- set_symbols("debug")

set_project("hazel")
set_languages("c++20")
-- local bin_dir = "bin/$(plat)/$(arch)"

if is_mode("debug") then
    add_defines("__HZ_DEBUG")
end

if is_plat("linux") then
    add_cxxflags("-Wall")
elseif is_plat("windows") then
    add_requires("opengl")
    add_packages("opengl")
    add_defines("NOMINMAX")
    add_cxxflags("/Zc:preprocessor")
    add_cxflags("/EHs")
    add_defines("UNICODE", "_UNICODE")
    add_cxflags("/utf-8")
    -- add_defines("_MSVC_LANG=202002L")
end

add_cxflags("-Wno-int-to-void-pointer-cast")

-- Why the vulkan require the MD not MT ?
-- And the vulkan still cannot link to my shared lib , and then I must set it as "static"
if is_plat("windows") then
    local runtimes = is_mode("debug") and "MDd" or "MD"
    set_runtimes(runtimes)
end



includes("./xmake/rules.lua")
includes("./xmake/tasks.lua")
includes("./src/xmake.lua")
