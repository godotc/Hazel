target("glad")
do
    set_kind("static")
    -- if is_mode("release") then
    -- set_symbols("hidden")
    -- set_optimize("fastest")
    -- set_strip("all")
    -- end
    add_files("./glad/src/glad.c")
    add_headerfiles("./glad/include/**.h", { public = true })
    add_includedirs("./glad/include", { public = true })
end

target("imgui-docking")
do
    set_kind("static")
    -- set_kind("shared")

    -- if is_mode("release") then
    -- set_symbols("hidden")
    -- set_optimize("fastest")
    -- set_strip("all")
    -- end

    add_files("./imgui-docking/*.cpp", "./imgui-docking/misc/cpp/*.cpp")
    add_headerfiles("./imgui-docking*.h", "./imgui-coking/misc/cpp/*.h", { public = true })
    add_includedirs("./imgui-docking", "./imgui-docking/backends", "./imgui-docking/misc/cpp", { public = true })

    -- opengl3  + glfw
    add_packages("glfw")
    add_files("./imgui-docking/backends/imgui_impl_opengl3.cpp")
    add_headerfiles("(./imgui-docking/backends/imgui_impl_opengl3.h)")
    add_files("./imgui-docking/backends/imgui_impl_glfw.cpp")
    add_headerfiles("(./imgui-docking/backends/imgui_impl_glfw.h)")
    -- if has_config("glad") then
    --    add_defines("IMGUI_IMPL_OPENGL_LOADER_GLAD")
    --    add_packages("glad")
    -- else
    add_headerfiles("(backends/imgui_impl_opengl3_loader.h)")
    -- end

    on_config(function(target)
        local kind = target:get("kind")
        local plat = target:is_plat("windows") and "windows" or "other"
        print(string.format("--[%s] type: %s, platform: %s", target:name(), kind, plat))
        if kind == "shared" then
            --TODO: somany things todo
            if target:is_plat("windows") then
                target:add("defines", "IMGUI_API=__declspec(dllexport)")
            else
                target:add("shflags", "-fPIC")
            end
        end
    end)




    if has_config("freetype") then
        add_files("misc/freetype/imgui_freetype.cpp")
        add_headerfiles("misc/freetype/imgui_freetype.h")
        add_packages("freetype")
        add_defines("IMGUI_ENABLE_FREETYPE")
    end

    if has_config("user_config") then
        local user_config = get_config("user_config")
        add_defines("IMGUI_USER_CONFIG=\"" .. user_config .. "\"")
    end

    if has_config("wchar32") then
        add_defines("IMGUI_USE_WCHAR32")
    end

    after_install(function(target)
        local config_file = path.join(target:installdir(), "include/imconfig.h")
        if has_config("wchar32") then
            io.gsub(config_file, "//#define IMGUI_USE_WCHAR32", "#define IMGUI_USE_WCHAR32")
        end
        if has_config("freetype") then
            io.gsub(config_file, "//#define IMGUI_ENABLE_FREETYPE", "#define IMGUI_ENABLE_FREETYPE")
        end
    end)
end

target("imguizmo")
do
    set_kind("static")

    -- if is_mode("release") then
    -- set_symbols("hidden")
    -- set_optimize("fastest")
    -- set_strip("all")
    -- end

    add_files("./imguizmo/*.cpp")
    add_headerfiles("./imguizmo/*.h", { public = true })
    -- add_packages("imgui")
    add_deps("imgui-docking")
    add_includedirs("./imguizmo", { public = true })
    -- add_defines("IMGUI_DEFINE_MATH_OPERATORS")
end



includes("./utility")
includes("./nelua")
includes("./smsharp")

