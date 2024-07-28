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

set_symbols("debug")

set_project("hazel")
set_languages("c++20")
-- local bin_dir = "bin/$(plat)/$(arch)"
-- set_targetdir(bin_dir)

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
	add_defines("UNICODE", "_UNICODE")
	add_cxflags("/utf-8")
    -- add_defines("_MSVC_LANG=202002L")
end

-- Why the vulkan require the MD not MT ?
-- And the vulkan still cannot link to my shared lib , and then I must set it as "static"
if is_plat("windows") then
    local runtimes = is_mode("debug") and "MDd" or "MD"
    set_runtimes(runtimes)
end



includes("./src")




-- on_config(function()
--     print("-- Loading manual testting...")
--     for _, file in ipairs(os.files("./test")) do
--         local name = path.basename(file)
--         target_name = "test." .. name
--         print(target_name)
--         target(target_name)
--         set_group("tst")
--         set_kind("binary")
--         add_files(file)
--         target_end()
--     end
-- end)


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
        local cmd  =import ("script.cmd")
        -- print(cmd)

        local profile = "debug"
        cmd.exec_cmds(
            "xmake f -c",
            string.format("xmake f -m %s ", profile), --toolchain=llvm",
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
        local cmd  =import ("script.cmd")

        local project =import("core.project.project")
        -- print(project)
        for targetname, target in pairs(project.targets()) do
            -- print(targetname)
            -- print(target)
            -- print(target:targetfile())
            -- print(target:basename())
            -- print(target:filename())
            -- print(target:linkname())
            -- print(target:targetdir())
            -- print(target:kind())
            -- return

            -- local target_name = arg[1] or "__DEFAULT_VAR__"
            -- local target_dir = arg[2] or "__DEFAULT_VAR__"
            -- local target_base_name = arg[3] or "__DEFAULT_VAr__"
            if target:kind() == "binary" then
                cmd.run_native_lua("script/vscode.lua", target:name(), target:targetdir() , target:basename(), target:type())
            end
        end
    end)
