add_requires("spdlog","glfw","glad")
add_packages("spdlog")

target("hazel")
    set_kind("shared")
    add_files("**.cpp")
    set_pcxxheader("hz_pch.h")

    add_packages("spdlog")
    add_packages("glfw", "glad")
    
    add_includedirs("./hazel")


    if is_os("linux") then
        add_cxflags("-fPIC")
    elseif is_os("windows") then
        add_defines("BUILD_SHARED_HAZEL")
    end

    if is_os("windows") then
        if is_mode("debug") then
            add_cxxflags("/MDd")
        elseif is_mode("release") then 
            add_cxxflags("/MT")
        end
    end