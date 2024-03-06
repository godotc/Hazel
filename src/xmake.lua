---@diagnostic disable: undefined-global

add_requires("glm", "fmt")
add_requires("spdlog", { configs = { fmt_eternal = true } })
add_requires("stb", { configs = { shared = true, debug = true } })

-- add_requires("entt 3.12", { configs = { debug = true } })
add_requires("entt 3.13", { configs = { debug = true } })
add_packages("entt", { public = true })

add_requires("yaml-cpp")
add_packages("yaml-cpp", { public = true })

add_packages("glm", "fmt", "spdlog", "stb")


includes("hazel", "editor-nut", "sandbox")
