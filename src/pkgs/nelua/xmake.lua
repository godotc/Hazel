add_requires("lua")
add_requires("gtest")

includes("./src/nelua_test/")

target("nelua")
do
    set_kind("shared")
    add_rules("microshit")
    add_deps("utitly", { public = true })

    -- set_options("microshit")


    add_packages("lua", { public = true })

    add_files("./src/nelua/**.cpp")
    add_headerfiles("./src/nelua/**.h", { public = true })
    add_includedirs("./src", { public = true })
end

