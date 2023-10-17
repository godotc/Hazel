add_rules("mode.debug", "mode.release")
-- add_rules("debug_game")

set_languages("c++latest")

set_targetdir("bin/$(plat)/$(mode)/$(arch)/")

if is_mode("debug") then
    add_defines("__HZ_DEBUG")
end

if is_plat("linux") then
    add_cxflags("-Wall")
end

if is_plat("windows") then
    add_requires("opengl")
    add_packages("opengl")
    add_defines("_MSVC_LANG=201402L")
end

includes("./src")

task("cpcm")
    set_menu {
        usage = "xmake cpcm"
    }
    on_run(function()
        local cmds =
        {
            "xmake f -c",
            "xmake f -m debug",
            "xmake project -k compile_commands",
        }
        for _, c in pairs(cmds) do
            print(os.exec(c))
        end
    end)
task_end()




