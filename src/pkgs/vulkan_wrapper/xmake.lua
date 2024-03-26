add_requires("vulkansdk", {
    configs = {
        -- runtimes = "MDd",
        utils = {
            "vulkan-1",
            "shaderc_shared",
            "spirv-cross-core",
            "spirv-cross-glsl",
        }
    }
})

---@format disable
target("vkwrapper")
    set_kind("shared")
    add_files("**.cpp")
    add_headerfiles("**.h")

    add_includedirs("./", {public = true})

    add_packages("vulkansdk", { public = true })
    -- set_runtimes("MT")

target_end()
