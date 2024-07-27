---@diagnostic disable: lowercase-global
function exec_cmds(...)
    local cmds = { ... }
    for _, c in pairs(cmds) do
        print(c)
        print(os.exec(c))
    end
end

function run_native_lua(file, ...)
    local cmd = string.format("lua %s", file)
    local arguments = { ... }
    for _, v in ipairs(arguments) do
        cmd = cmd .. ' ' .. v .. ' '
    end
    print(os.exec(cmd))
end
