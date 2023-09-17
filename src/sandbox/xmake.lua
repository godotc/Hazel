
target("sb")
	set_kind("binary")
    set_basename("sandbox")

	add_files("**.cpp")

    add_packages("imgui")

    add_deps("hazel")

    add_defines("HAZEL_LINKAGE_PROGRAM")
	
