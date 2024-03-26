--[[
/**
 *  Author: @godot42
 *  Create Time: 2024-03-11 22:31:15
 *  Modified by: @godot42
 *  Modified time: 2024-03-27 02:40:05
 *  Description:
 */
 ]]

---@diagnostic disable: undefined-global
add_requires("glfw", {
    configs = {
        -- debug = true,
    }
})
add_requires("glad", { configs = { debug = true, pic = ture } })

add_requires("imgui docking", {
    system = false,
    configs = {
        debug = true,
        opengl3 = true,
        glfw = true,
    },
})


-- add_requires("vulkansdk", {
--     configs = {
--         -- runtimes = is_mode("debug") and "MDd" or "MD",
--         -- runtimes = "MDd",
--         -- vc_runtimes = "MDd",

--         shared = true,

--         utils = is_mode("debug") and {
--             "vulkan-1",
--             "shaderc_sharedd",
--             "spirv-cross-cored",
--             "spirv-cross-glsld",
--         } or {
--             "vulkan-1",
--             "shaderc_shared",
--             "spirv-cross-core",
--             "spirv-cross-glsl",
--         }
--     }
-- })
-- add_packages("vulkansdk",{public=true})
-- if is_plat("windows") then
--     add_links("libucrt.lib")
--     add_links("libcpmt.lib")
-- end
-- add_requires("shaderc")
-- add_requires("volk")

---@format disable
target("hazel")
    set_kind("shared")


    add_deps("vkwrapper")
    add_deps("imguizmo")
    -- add_cxflags("/DYNAMICBASE")

    add_includedirs("./", { public = true })
    add_headerfiles("**.h")
    set_pcxxheader("hz_pch.h")
    -- add_files("hz_pch.cpp")

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

    add_packages("spdlog", "glad")
    add_packages("glfw", { public = true })

    add_packages("imgui", {public=true})


    -- set_runtimes("MT")
    -- if is_os("windows") then
    --     if is_mode("debug") then
    --         set_runtimes("MDd")
    --     elseif is_mode("release") then
    --         set_runtimes("/MD")
    --     end
    -- end



    on_config(function(target)
        if target:get("kind") == "shared" then
            print("--[" .. target:name() .. "] is the shared library")
            if target:is_plat("windows") then
                if is_mode("debug") then
                    -- target:set("runtimes","MDd")
                else
                    -- target:set("runtimes","MD")
                end
                target:add("defines", "SHARED_PROGRAM")
                target:add("defines", "BUILD_SHARED_HAZEL")
            else
                target:add("shflags", "-fPIC")
            end
        else
            if is_mode("debug") then
                -- target:set("runtimes","MTd")
            else
                -- target:set("runtimes","MT")
            end
        end
    end)
target_end()
