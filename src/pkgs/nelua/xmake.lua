add_requires("lua")



target("nelua")
do
    set_kind("shared")
    add_rules("microshit")
    add_deps("utitly", { public = true })

    -- set_options("microshit")


    add_packages("lua", { public = true })

    add_files("./src/**.cpp")
    add_headerfiles("./src/**.h", { public = true })
    add_includedirs("./src", { public = true })
end
