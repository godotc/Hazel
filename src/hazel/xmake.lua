---@diagnostic disable: undefined-global
add_requires("glfw", {
    configs = {
        debug = true,
    }
})
add_requires("glad", { configs = { debug = true } })



add_requires("imgui docking", {
    system = false,
    configs = {
        debug = true,
        opengl3 = true,
        glfw = true,
    },
})

add_deps("imguizmo")

---@format disable
target("hazel")
    set_kind("shared")

    add_includedirs("./", { public = true })
    add_headerfiles("**.h")
    set_pcxxheader("hz_pch.h")
    add_files("hz_pch.cpp")

    add_files("config/**.cpp")
    add_files("hazel/**.cpp")
    add_files("utils/**.cpp")
    add_files("math/**.cpp")
    add_files("platform/opengl/**.cpp")

    -- TODO: Sperator the windows and linux platfrom files
    add_files("platform/windows/**.cpp")
    add_files("platform/linux/**.cpp")
    if is_plat("windows") then
        add_links("Comdlg32")
    end

    add_packages("spdlog", "glad")
    add_packages("glfw", { public = true })

    add_packages("imgui", {public=true})
    add_packages("imguizmo", {public=true})


    if is_os("windows") then
        if is_mode("debug") then
            --             add_cxxflags("/MDd")
        elseif is_mode("release") then
            --             add_cxxflags("/MT")
        end
    end


    on_config(function(target)
        if target:get("kind") == "shared" then
            print("--[" .. target:name() .. "] is the shared library")
            if target:is_plat("windows") then
                target:add("defines", "BUILD_SHARED_HAZEL")
            else
                target:add("shflags", "-fPIC")
            end
        end
    end)
target_end()
