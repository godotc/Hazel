--[[
/**
 *  Author: @godot42
 *  Create Time: 2024-03-11 22:31:15
 *  Modified by: @godot42
 *  Modified time: 2024-07-26 00:10:53
 *  Description:
 */
 ]]

add_requires("glfw", {
    configs = { -- debug = true,
    }
})

add_requires("imgui docking", {
    system = false,
    configs = {
        debug = true,
        opengl3 = true,
        glfw = true,
    },
})


add_requires("vulkansdk", {
    configs = {
        -- runtimes = is_mode("debug") and "MDd" or "MD",
        -- shared = true,
        debug=false,
        utils = is_mode("debug") and {
            "vulkan-1",
            "shaderc_sharedd",
            "spirv-cross-cored",
            "spirv-cross-glsld",
        } or {
            "vulkan-1",
            "shaderc_shared",
            "spirv-cross-core",
            "spirv-cross-glsl",
        }
    }
})
-- add_packages("vulkansdk",{public=true})
-- if is_plat("windows") then
--     add_links("libucrt.lib")
--     add_links("libcpmt.lib")
-- end
-- add_requires("shaderc")
-- add_requires("volk")

---@format disable
target("hazel")
    -- set_kind("shared")
    set_kind("static")


    -- add_deps("vkwrapper")
    add_deps("imguizmo")
    add_deps("glad")
    -- add_cxflags("/DYNAMICBASE")

    add_includedirs("./", { public = true })
    add_headerfiles("**.h")
    set_pcxxheader("hz_pch.h")

    -- add_files("config/**.cpp")
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

    add_packages("glfw", { public = true })
    add_packages("imgui", {public=true})


    add_packages("vulkansdk", { public = true })

    on_config(function(target)
        local kind = target:get("kind")
        local plat = target:is_plat("windows") and "windows" or "other"
        print(string.format("--[%s] type: %s, platform: %s", target:name(), kind , plat))

        if kind == "shared" then
            if target:is_plat("windows") then
                target:add("defines", "SHARED_PROGRAM")
                target:add("defines", "BUILD_SHARED_HAZEL")
            else
                target:add("shflags", "-fPIC")
            end
        end

    end)
target_end()
