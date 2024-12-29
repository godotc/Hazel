add_requires("lua")

target("nelua")
do
    set_kind("binary")

    add_packages("lua")

    add_files("./src/**.cpp")
end
