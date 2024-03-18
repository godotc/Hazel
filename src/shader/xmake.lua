
target("test")
    set_kind("binary")
    add_rules("utils.glsl2spv", {outputdir = "build"})
    add_files("src/*.c")
    add_files("src/*.vert", "src/*.frag")
    add_packages("glslang")