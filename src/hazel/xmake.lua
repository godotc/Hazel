add_requires("spdlog")
add_packages("spdlog")

target("hazel")
    set_kind("shared")
    add_files("**.cpp")

    add_packages("spdlog")

    if is_os("linux") then
        add_cxflags("-fPIC")
    elseif is_os("windows") then
        add_defines("BUILD_SHARED_HAZEL")
    end