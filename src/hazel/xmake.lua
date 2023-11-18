add_requires("spdlog")
add_requires( "glfw", {configs={debug=true}})
add_requires( "glad", {configs={debug=true}})

add_packages("spdlog")

target("hazel")
    set_kind("shared")

    add_includedirs("./",{public = true})

    add_files("**.cpp")
    add_headerfiles("**.h")
    set_pcxxheader("hz_pch.h")

    add_packages("spdlog","glad")
    add_packages("glfw", {public=true})

    add_deps("imgui-docking", {public=true})


    if is_os("windows") then
        if is_mode("debug") then
--             add_cxxflags("/MDd")
        elseif is_mode("release") then
--             add_cxxflags("/MT")
        end
    end


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

