add_requires("dotnet")

target("sm")
do
    set_kind("binary")

    add_packages("dotnet")

    add_files("./src/**.cpp")
end
