
target("sb")
	set_kind("binary")
    set_basename("sandbox.$(mode).$(arch)")


	add_files("**.cpp")

    -- add_packages("glfw"/core.h)
    add_deps("hazel")
    add_defines("HAZEL_LINKAGE_PROGRAM")
	
