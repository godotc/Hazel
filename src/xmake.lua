
add_requires("fmt", "glm", "spdlog")
add_requires("stb", {configs = {shared=true,  debug=true}})

add_requires("entt" ,{configs = {debug=true}})
add_packages("entt", {public=true})

add_packages("fmt", "glm", "spdlog","stb")

-- FIXME:
-- 1. Why cannot working on windows
-- 2. Crashing while non debug for imgui
-- add_requires("imgui docking", {configs = {shared=false,  debug=true,  opengl3 = true, glfw=true }})
-- add_packages("imgui", {debug=false})


includes("hazel","editor-nut","sandbox")
