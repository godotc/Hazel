target("nelua_test")
do
    set_kind("binary")
    print("script dir:", os.scriptdir())
    set_rundir(os.scriptdir())

    -- before_run(function()
    --     print("curdir ==  $(curdir)")
    --     print("scriptdir == $(scriptdir)")
    -- end)

    add_deps("nelua", { public = true })

    add_files("./**.cpp")
    add_packages("gtest", { public = true })
end
