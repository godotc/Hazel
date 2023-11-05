

target("nut")
    set_kind("binary")
    set_basename("hazel-editor-nut")

    add_deps("hazel")

    add_headerfiles("**.h")
    add_files("**.cpp")

    add_defines("HAZEL_ENTRY_PROGRAM")