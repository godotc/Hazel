add_rules("mode.debug", "mode.release")

set_languages("c++20")

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
end


includes("./src")






