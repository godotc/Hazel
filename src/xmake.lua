
add_requires("fmt", "glm", "spdlog")
add_requires("imgui docking", {configs = {shared=true,  debug=true,  opengl3 = true, glfw=true }})
add_requires("stb", {configs = {shared=true,  debug=true}})


add_packages("fmt", "glm", "spdlog","stb")

add_packages("imgui", {shared=true, public = true})


includes("hazel","sandbox")
