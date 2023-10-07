
add_requires("fmt", "glm", "spdlog")
add_requires("imgui docking", {configs = {shared=false,  debug=true,  opengl3 = true, glfw=true }})
add_requires("stb", {configs = {shared=false,  debug=true}})

add_packages("fmt", "glm", "imgui", "spdlog","stb")

add_includedirs(".")
add_includedirs("./hazel")

includes("hazel","sandbox")
