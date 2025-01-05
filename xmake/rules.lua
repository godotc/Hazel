
rule("microshit")
do
    on_load(function(target)
        -- print(project:directory())

        -- import("core.project.project")
        -- print(target:name())
        -- local api_name = string.upper(target:name()) .. "API"


        -- if target:is_kind("binary") or target:is_kind("static") then
        --     -- add_defines(api_name, "")
        -- elseif target:is_kind("shared") then
        --     add_defines(api_name, "MICROSHIT_EXPORTS")
        -- end


        local kind = target:get("kind")

        -- print (target)
        print(target:name())
        print(target:type())

        if kind == "shared" then
            -- target:add_defines("-DTYPE_BUILD_SHARED")
            -- target:add_cxxflags("-DTYPE_BUILD_SHARED")
            print("--[" .. target:name() .. "] add defines-> TYPE_BUILD_SHARED")
            target:add("defines", "TYPE_BUILD_SHARED")
        end
    end)
end