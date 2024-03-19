--[[
/**
 *  Author: @godot42
 *  Create Time: 2024-03-11 22:31:15
 *  Modified by: @godot42
 *  Modified time: 2024-03-20 01:13:43
 *  Description:
 */
 ]]

---@diagnostic disable: undefined-global
add_requires("glfw", {
    configs = {
        debug = true,
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

add_deps("imguizmo")

-- 2024/3/15 integrate vulkan and spriv system
-- add_requires("vulkansdk", {
--     configs = {
--         shared = true,
--         utils = {
--             "shaderc_shared",
--             "spirv-cross-core",
--             "spirv-cross-glsld", }
--     }
-- })
-- add_requires("shaderc")

-- package("my_vulkansdk")
--     set_homepage("https://www.lunarg.com/vulkan-sdk/")
--     set_description("LunarG Vulkan® SDK")

--     add_configs("shared", {description = "Build shared library.", default = true, type = "boolean", readonly = true})
--     add_configs("utils",  {description = "Enabled vulkan utilities.", default = {}, type = "table"})

--     on_load(function (package)
--         import("detect.sdks.find_vulkansdk")
--         local vulkansdk = find_vulkansdk()
--         if vulkansdk then
--             package:addenv("PATH", vulkansdk.bindir)
--             print("...vulkansdk.bindir")
--         end
--     end)

--     on_fetch(function (package, opt)
--         if opt.system then
--             import("detect.sdks.find_vulkansdk")
--             import("lib.detect.find_library")

--             local vulkansdk = find_vulkansdk()
--             if vulkansdk then
--                 local result = {includedirs = vulkansdk.includedirs, linkdirs = vulkansdk.linkdirs, links = {}}
--                 local utils = package:config("utils")
--                 table.insert(utils, package:is_plat("windows") and "vulkan-1" or "vulkan")

--                 for _, util in ipairs(utils) do
--                     if not find_library(util, vulkansdk.linkdirs) then
--                         wprint(format("The library %s for %s is not found!", util, package:arch()))
--                         return
--                     end
--                     table.insert(result.links, util)
--                 end
--                 return result
--             end
--         end
--     end)
-- package_end()



---@format disable
target("hazel")

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


    -- add_packages("vulkansdk",{public=true})

    set_kind("shared")
    local vulkan = os.getenv("VUKLAN_SDK") or os.getenv("VK_SDK_PATH")
    print(vulkan)
    if is_plat("windows") then
        add_includedirs(vulkan.."\\Include")
        local vulkan_dll_dir ={  vulkan.."\\Bin", vulkan.."\\Lib" };
        for k,v in pairs(vulkan_dll_dir) do
            add_linkdirs(v)
            add_rpathdirs(v)
            print(v)
        end
    end

    if is_plat("windows") then
        if is_mode("debug") then
            add_shflags( 
                    "vulkan-1",

                    "shaderc_sharedd",
                    "spirv-cross-cored",
                    "spirv-cross-glsld"
                )
            add_links(
                    "vulkan-1",

                    "shaderc_sharedd",
                    "spirv-cross-cored",
                    "spirv-cross-glsld"
                )
        end
    end

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
                target:add("defines", "BUILD_SHARED_HAZEL")
            else
                target:add("shflags", "-fPIC")
            end
        end
    end)
target_end()
