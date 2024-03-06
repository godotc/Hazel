---@diagnostic disable: undefined-global
add_rules("mode.debug", "mode.release")
-- add_rules("debug_game")

set_languages("c++20")
set_targetdir("bin/$(plat)/$(mode)/$(arch)/")
set_symbols("debug")

set_project("hazel")

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


includes("./src")



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


task("test")
set_menu {}
on_run(function()
    -- print(os.exec("xmake f -m debug --test=y")
    os.exec("xmake f -m debug")
    print(os.exec("xmake build -g test"))
    print(os.exec("xmake run -g test"))
end)



task("cpcm")
set_menu {
    usage = "xmake cpcm"
}
on_run(function()
    local cmds =
    {
        "xmake f -c",
        "xmake f -m debug ", --toolchain=llvm",
        "xmake project -k compile_commands",
    }
    for _, c in pairs(cmds) do
        print(os.exec(c))
    end
end)



task("targets")
set_menu {}
on_run(function()
    for targetname, target in pairs(project.targets()) do
        print(target:targetfile())
    end
end)
