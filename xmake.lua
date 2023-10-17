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



for _,file in ipairs(os.files("./test")) do
    local name = path.basename(file)
    target("test."..name)
        set_group("test")
        set_kind("binary")
        add_files(file)
    target_end()
end


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




task("test")
    set_menu{}
    on_run(function()
--         print(os.exec("xmake f -m debug --test=y")
            os.exec("xmake f -m debug")
            print(os.exec("xmake build -g test"))
            print(os.exec("xmake run -g test"))
    end)
task_end()
