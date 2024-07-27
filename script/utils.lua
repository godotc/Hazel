Utils = Utils or {}
local function get_indent(count)
    local s = ''
    for _ = 1, count do
        s = s .. '\t'
    end
    return s;
end
-- print(get_indent(0) .. "123")
-- print(get_indent(1) .. "123")

local function dump_internal(o, ctx)
    get_indent(ctx.n_indent)

    if type(o) == 'table' then
        local s = "{\n"

        ctx.n_indent = ctx.n_indent + 1 -- indent one more
        for k, v in pairs(o) do
            -- pre indent
            s = s .. get_indent(ctx.n_indent)

            -- key
            if type(k) ~= 'number' then
                k = '"' .. k .. '"'
            end
            s = s .. '[' .. k .. '] = '

            -- value
            s = s .. dump_internal(v, ctx) .. ',\n'
        end
        ctx.n_indent = ctx.n_indent - 1 -- pop one indent

        s = s .. get_indent(ctx.n_indent) .. "}"

        return s
    else
        return tostring(o)
    end
end

function Utils.Dump(o)
    return dump_internal(o, { n_indent = 0 })
end

function Utils.DumpPrint(o)
    print(Utils.Dump(o))
end

function Utils.DeepCopy(origin)
    local copy
    if type(origin) == 'table' then
        copy = {}
        for k, v in next, origin, nil do
            copy[Utils.DeepCopy(k)] = Utils.DeepCopy(v)
        end
        setmetatable(copy, Utils.DeepCopy(getmetatable(origin)))
    else
        copy = origin
    end
    return copy
end

-- local tbl = {
--     name = "",
--     type = "lldb",
--     request = "launch",
--     args = {},
--     cwd = "${workspaceFolder}",
--     windows = {
--         program = ""
--     },
--     preLaunchTask = pre_launch_task,
-- }

-- Utils.DumpPrint(tbl)
