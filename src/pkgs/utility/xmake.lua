target("utitly")
do
    set_kind("static")
    set_languages("c++20")
    add_files("./src/**.cpp")
    add_headerfiles("./src/**.h", { public = true })
    add_includedirs("./src", { public = true })
end
