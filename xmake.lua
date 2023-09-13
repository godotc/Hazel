add_rules("mode.debug", "mode.release")


target("hazel")
    set_kind("binary")
    add_files("src/*.cpp")


add

