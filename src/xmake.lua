
add_requires("glm","fmt")
add_requires("spdlog", {configs= {fmt_eternal=true}})
add_requires("stb", {configs = {shared=true,  debug=true}})

add_requires("entt" ,{configs = {debug=true}})
add_packages("entt", {public=true})

add_packages("glm","fmt", "spdlog","stb")

-- FIXME:
-- 1. Why cannot working on windows
-- 2. Crashing while non debug for imgui
-- add_requires("imgui docking", {configs = {shared=false,  debug=true,  opengl3 = true, glfw=true }})
-- add_packages("imgui", {debug=false})


includes("hazel","editor-nut","sandbox")

if is_plat("windows") then
    add_defines("NOMINMAX")
end