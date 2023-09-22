add_requires("spdlog","glad")
add_requires( "glfw", {configs={debug=true}})
add_packages("spdlog")


target("hazel")
    set_kind("shared")

    add_files("**.cpp")
    add_headerfiles("**.h")
    set_pcxxheader("hz_pch.h")

    add_packages("fmt")
    add_packages("spdlog")
    add_packages("glfw", "glad")

    add_includedirs("./hazel")

    if is_os("windows") then
        if is_mode("debug") then
--             add_cxxflags("/MDd")
        elseif is_mode("release") then
--             add_cxxflags("/MT")
        end
    end

    on_load(function (target)

    end)

    on_config(function (target)
        if target:get("kind")=="shared" then
            print("--["..target:name().."] is the shared library")
            if target:is_plat("windows") then
                target:add("defines", "BUILD_SHARED_HAZEL")
            else
                target:add("shflags","-fPIC")
            end
        end
    end)

