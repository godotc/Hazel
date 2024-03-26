---@diagnostic disable: undefined-global

add_requires("fmt", { system = false })
add_packages("fmt")

add_requires("glm")
add_requires("spdlog", { system = false, configs = { fmt_eternal = true } })
add_requires("stb", { configs = { shared = true, debug = true } })
add_packages("glm", "fmt", "spdlog", "stb")

-- add_requires("entt 3.12", { configs = { debug = true } })
add_requires("entt 3.13", { configs = { debug = true } })
add_packages("entt", { public = true })

add_requires("yaml-cpp")
add_packages("yaml-cpp", { public = true })


includes("pkgs")
includes("hazel", "editor-nut", "sandbox")

