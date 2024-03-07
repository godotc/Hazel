---@format disable
target("sb")
	set_kind("binary")
    set_basename("sandbox")
	add_files("**.cpp")

    add_deps("hazel")
    add_defines("HAZEL_ENTRY_PROGRAM")
target_end()
