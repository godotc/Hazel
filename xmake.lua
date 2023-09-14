add_rules("mode.debug", "mode.release")

set_languages("c++20")

set_targetdir("bin//$(plat)/$(mode)/$(arch)/")

add_requires("fmt")
add_packages("fmt")



includes("./src")
includes("./pkgs/m_log")

add_includedirs("./pkgs/m_log")





