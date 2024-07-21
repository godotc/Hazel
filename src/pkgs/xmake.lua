target("imguizmo")
    set_kind("static")
    add_files("./imguizmo/*.cpp")
    add_headerfiles("./imguizmo/*.h", {public=true})
    add_packages("imgui")
    add_includedirs("./imguizmo", {public=true})
-- add_defines("IMGUI_DEFINE_MATH_OPERATORS")
target_end()


-- includes("vulkan_wrapper")

target("glad")
    set_kind("static")
    add_files("./glad/src/glad.c")
    add_headerfiles("./glad/include/**.h", {public=true})
    add_includedirs("./glad/include", {public=true})
target_end()

