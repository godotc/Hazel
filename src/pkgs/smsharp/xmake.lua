add_requires("dotnet")


rule("dotnet")
do
    set_extensions(".cs")

    on_build(function(target)
        import("core.project.project")

        -- cmd.exec("dotnet", "build")
    end)


    after_build(function(target)
        import("core.project.project")
        -- os.cp(target:targetfile(), target:targetdir())
    end)
end


target("sm")
do
    add_rules("dotnet")
    set_kind("binary")

    add_packages("dotnet")

    add_files("./src/**.cpp")
end
