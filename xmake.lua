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
-- set_targetdir(bin_dir)

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


task("test")
do
    set_menu {}
    on_run(function()
        for targetname, target in pairs(project.targets()) do
            print(targetname)
            print(target)
            print(target:targetfile())
            print(target:basename())
            print(target:filename())
            print(target:linkname())
            print(target:targetdir())
            print(target:kind())
            -- return

            -- local target_name = arg[1] or "__DEFAULT_VAR__"
            -- local target_dir = arg[2] or "__DEFAULT_VAR__"
            -- local target_base_name = arg[3] or "__DEFAULT_VAr__"
        end

        -- exec_cmds(
        --     "xmake f -m debug --test=y",
        --     -- "xmake f -m debug",
        --     "xmake build -g test",
        --     "xmake run -g test"
        -- )
    end)
end

task("cpcm")
do
    set_menu {
        usage = "xmake cpcm",
        options = {
            { nil, "rule", "v", "debug", "the rule to config build mode " }
        }
    }
    on_run(function()
        local cmd     = import("script.cmd")
        local opt     = import("core.base.option")
        -- print(cmd)

        local profile = opt.get("rule")
        cmd.exec_cmds(
            "xmake f -c",
            string.format("xmake f -m %s ", profile), --toolchain=llvm",
            "xmake project -k compile_commands"
        )
    end)
end

task("targets")
do
    set_menu {}
    on_run(function()
        for targetname, target in pairs(project.targets()) do
            print(target:targetfile())
        end
    end)
end

task("vscode")
do
    local usage_str = "xmake vscode <toolchain (clang/gcc/msvc)>"
    set_menu {
        usage = usage_str,
        description = "generate and update vscode project files",
        options = {
            {},
            { nil, "toolchain", "v", nil, "the toolchain to use, clang/gcc/msvc" },
        }
    }
    on_run(function()
        local cmd       = import("script.cmd")
        local opt       = import("core.base.option")
        local project   = import("core.project.project")

        local toolchain = opt.get("toolchain")
        if toolchain == nil or toolchain == "" then
            print(usage_str)
            return
        end

        -- print(project)
        for target_name, target in pairs(project.targets()) do
            -- print(targetname) -- nut
            -- print(target)
            -- print(target:targetfile())
            local target_basename = target:basename() -- hazel-editor-nut
            -- print(basename)
            -- print(target:filename())
            -- print(target:linkname())
            local target_dir = target:targetdir() -- build\windows\x64
            -- print(targetdir)
            -- print(target:kind())
            -- print(target:toolchains())
            -- do return end

            -- local toolchain = arg[1] or "lldb"
            if target:kind() == "binary" then
                -- cmd.exec("lua", "script/vscode.lua", target:name(), target:targetdir(), target:basename(), target:type())
                -- TODO: drop lua script, use python instead. It could safe much more times for trivial works...
                cmd.exec("python3", "script/update_vscode_debug_misc.py", toolchain, target_name, target_basename,
                    target_dir)
            end
        end
    end)
end



task("t")
do
    set_menu {}
    on_run(function()
        local cmds = import("script.cmd")
        local project = import("core.project.project")
        for targetname, target in pairs(project.targets()) do
            print(targetname)
            -- print(target)
            print(target:targetfile())
            local a, b, c = target:tool("cxx")
            print(a)
            print(b)
            print(c)
            print(c.name)
            print(c.arch)

            print(target:basename())
            print(target:filename())
            print(target:linkname())
            print(target:targetdir())
            print(target:kind())
            break
        end

        --  print("........................")
        --  local ret = cmds.exec_cmds("xmake show -l toolchains")
        --  print(ret)
    end)
end
