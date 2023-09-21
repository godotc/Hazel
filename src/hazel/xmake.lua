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

    if not is_kind("static") then
        if is_os("windows") then
            add_defines("BUILD_SHARED_HAZEL")
        else
            add_shflags("-fPIC")
        end
    end
