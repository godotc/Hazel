---@format disable
target("imguizmo")
    set_kind("static")
    add_files("./imguizmo/*.cpp")
    add_headerfiles("./imguizmo/*.h", {public=true})
    add_packages("imgui")
    add_includedirs("./imguizmo", {public=true})
-- add_defines("IMGUI_DEFINE_MATH_OPERATORS")
target_end()


-- includes("vulkan_wrapper")
