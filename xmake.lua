--[[
/**
 *  Author: @godot42
 *  Create Time: 2024-07-21 01:48:13
 *  Modified by: @godot42
 *  Modified time: 2024-07-22 03:24:35
 *  Description:
 */
 ]]



add_rules("mode.debug", "mode.release")
-- add_rules("debug_game")

set_symbols("debug")

set_project("hazel")
set_languages("c++20")

if is_mode("debug") then
    add_defines("__HZ_DEBUG")
end

if is_plat("linux") then
    add_cxflags("-Wall")
elseif is_plat("windows") then
    add_requires("opengl")
    add_packages("opengl")
    add_defines("NOMINMAX")
    add_cxflags("/Zc:preprocessor")
    add_cxflags("/EHs")
    -- add_defines("_MSVC_LANG=202002L")
end

-- Why the vulkan require the MD not MT ?
-- And the vulkan still cannot link to my shared lib , and then I must set it as "static"
if is_plat("windows") then
    local runtimes = is_mode("debug") and "MDd" or "MD"
    set_runtimes(runtimes)
end



includes("./src")

local function exec_cmds(...)
    for _, c in pairs(...) do
        print(os.exec(c))
    end
end


on_config(function()
    print("-- Loading manual testting...")
    for _, file in ipairs(os.files("./test")) do
        local name = path.basename(file)
        target_name = "test." .. name
        print(target_name)
        target(target_name)
        set_group("tst")
        set_kind("binary")
        add_files(file)
        target_end()
    end
end)


---@format disable
task("test")
    set_menu {}
    on_run(function()
        exec_cmds(
            "xmake f -m debug --test=y",
            -- "xmake f -m debug",
            "xmake build -g test",
            "xmake run -g test"
        )
    end)

task("cpcm")
    set_menu {
        usage = "xmake cpcm"
    }
    on_run(function()
        local profile = "debug"
        exec_cmds(
            "xmake f -c",
            format("xmake f -m %s ", profile), --toolchain=llvm",
            "xmake project -k compile_commands"
        )
    end)

task("targets")
    set_menu {}
    on_run(function()
        for targetname, target in pairs(project.targets()) do
            print(target:targetfile())
        end
    end)

task("vscode")
    set_menu{ }
    on_run(function ()
        import("script.vscode")
        vscode.update_launch_profile(bin_dir)
    end)