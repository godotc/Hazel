--[[
/**
 *  Author: @godot42
 *  Create Time: 2024-03-11 22:31:15
 *  Modified by: @godot42
 *  Modified time: 2024-03-19 02:58:34
 *  Description:
 */
 ]]

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

-- 2024/3/15 integrate vulkan and spriv system
add_requires("vulkansdk")
-- add_requires("shaderc")



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


    add_packages("vulkansdk",{public=true})
    if is_plat("windows") then
        add_links(
            "vulkan-1.lib",
            -- "VkLayer_utils.lib",

            "shaderc_shared.lib",
            "spirv-cross-core.lib",
            "spirv-cross-glsl.lib"
        )
    end

    if is_os("windows") then
        if is_mode("debug") then
            set_runtimes("MDd")
        elseif is_mode("release") then
            set_runtimes("/MD")
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
