---@format disable
target("nut")
    set_kind("binary")
    set_basename("hazel-editor-nut")

    add_deps("hazel")

    add_headerfiles("**.h")
    add_files("**.cpp")


    on_config(function(target)
        local kind = target:get("kind")
        print(format("--[%s] type: %s", target:name(), kind))
        if kind == "binary" then
            target:add("defines", "HAZEL_ENTRY_PROGRAM")
        end
    end)


target_end()
