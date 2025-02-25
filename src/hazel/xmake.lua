--[[
/**
 *  Author: @godot42
 *  Create Time: 2024-03-11 22:31:15
 *  Modified by: @godot42
 *  Modified time: 2024-07-28 17:46:42
 *  Description:
 */
 ]]

add_requires("glfw", {
    configs = { -- debug = true,
    }
})

-- add_requires("imgui docking", {
--     system = false,
--     configs = {
--         debug = true,
--         opengl3 = true,
--         glfw = true,
--     },
-- })


add_requires("vulkansdk", {
    configs = {
        -- runtimes = is_mode("debug") and "MDd" or "MD",
        -- shared = true,
        debug = false,
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

add_requires("nlohmann_json")
add_requires("box2d v2.4.2")
-- add_requires("openssl")

-- add_requires("dotnet 8.0")
-- add_requires("mono 6.12")


target("hazel")
do
    -- set_kind("shared")
    set_kind("static")


    add_deps("imguizmo")
    add_deps("glad")
    add_deps("imgui-docking")

    add_deps("nelua")

	add_deps("spirv-cross")

    add_includedirs("./", { public = true })
    add_headerfiles("**.h")
    set_pcxxheader("hz_pch.h")

    -- add_files("config/**.cpp")
    add_files("hazel/**.cpp")
    add_files("utils/**.cpp")
    add_files("math/**.cpp")
    add_files("platform/opengl/**.cpp")
    add_files("platform/glfw/**.cpp")

    -- TODO: Sperator the windows and linux platfrom files
    if is_plat("windows") then
        add_files("platform/windows/**.cpp")
    elseif is_plat("linux") then
        add_files("platform/linux/**.cpp")
    end

    if is_plat("windows") then
        add_links("Comdlg32")
    end

    add_packages("glfw", { public = true })
    add_packages("vulkansdk", { public = true })
    add_packages("box2d")
    add_packages("nlohmann_json", { public = true })
    -- add_packages("openssl", { public = true })
    -- add_packages("dotnet", { public = true })
    -- add_packages("mono", { public = true })

    on_config(function(target)
        local kind = target:get("kind")
        local plat = target:is_plat("windows") and "windows" or "other"
        print(string.format("--[%s] type: %s, platform: %s", target:name(), kind, plat))


        if kind == "shared" then
            if target:is_plat("windows") then
                target:add("defines", "SHARED_PROGRAM")
                target:add("defines", "BUILD_SHARED_HAZEL")
            else
                target:add("shflags", "-fPIC")
            end
        end
    end)
end
target_end()
