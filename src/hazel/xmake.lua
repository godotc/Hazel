add_requires("spdlog","glad")
add_requires( "glfw", {configs={debug=true}})
add_packages("spdlog")


target("hazel")
    set_kind("shared")

    add_files("**.cpp")
    add_headerfiles("**.h")
    set_pcxxheader("hz_pch.h")

    add_packages("spdlog")
    add_packages("glfw", "glad")

    add_includedirs("./hazel")


    if is_os("windows") then
        if is_mode("debug") then
            add_cxxflags("/MDd")
        elseif is_mode("release") then
            add_cxxflags("/MT")
        end
    end

    if is_kind("shared") then
        if is_plat("linux") then
            add_shflags("-fPIC")
        elseif is_plat("windows") then
            add_defines("BUILD_SHARED_HAZEL")
        end
    end
