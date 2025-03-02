---@diagnostic disable: undefined-global

add_requires("fmt 11.0.1", {
    system = false,
    configs = {
        header_only = true,
        cxxflags = "/utf-8"
    }
})
add_requires("glm", { configs = { header_only = true, cxx_standard = "20" } })
add_requires("spdlog v1.13.0", { system = false, configs = { fmt_eternal = true } })
add_requires("stb", { configs = { shared = true, debug = true } })
add_requires("toml++")

add_packages(
    "fmt",
    "glm",
    "fmt",
    "spdlog",
    "stb",
    "toml++"
)

add_requires("utfcpp")
add_packages("utfcpp", { public = true })

-- add_requires("entt 3.12", { configs = { debug = true } })
add_requires("entt 3.13", { configs = { debug = true, cxxflgs = "-std=c++20" } })
add_packages("entt", { public = true })

add_requires("yaml-cpp", {
    configs = {
        debug = is_mode("debug"),
        cxxflags = is_plat("windows") and "/FS",
    }
})
add_packages("yaml-cpp", { public = true })


includes("pkgs")
includes(
    "hazel/hazel.xmake.lua",
    "editor-nut"
-- ,"sandbox"
)
