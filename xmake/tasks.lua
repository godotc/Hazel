do -- grab all cpp file under test folder as a target
    local bDebug = false
    local test_files = os.files(os.projectdir() .. "/test/*.cpp")
    for _, file in ipairs(test_files) do
        local name = path.basename(file)
        target_name = "test." .. name
        target(target_name)
        do
            if bDebug then
                print("add test unit:", target_name)
            end
            set_group("test")
            set_kind("binary")
            add_files(file)
            target_end()
        end
    end
end
task("test")
do
    set_menu {
        usage = "xmake test",
        options = {
            { nil, "rule", "v", "debug", "the rule to config build mode " }
        }
    }
    on_run(function()
        os.exec("xmake b -g test")
        os.exec("xmake r -g test")
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
        local cmd     = import("cmd") -- works
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
        local cmd       = import("xmake.cmd")
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
