
add_requires("fmt", "glm", "spdlog")
add_requires("imgui docking",  {configs = {shared=true,  debug=true,  opengl3 = true, glfw=true }})

add_packages("fmt", "glm", "imgui", "spdlog")


add_includedirs(".")
add_includedirs("./hazel")

includes("hazel","sandbox")
