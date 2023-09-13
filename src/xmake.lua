
add_includedirs("./hazel")


target("hazel")
    set_kind("shared")
    add_files("hazel/**.cpp")
	add_deps("m_log")

    if is_os("linux") then
        add_cxflags("-fPIC")
    elseif is_os("windows") then
        add_defines("BUILD_SHARED_HAZEL")
    end




target("sandbox")
	set_kind("binary")
	add_files("sandbox/**.cpp")
	
