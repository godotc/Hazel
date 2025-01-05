---@diagnostic disable: lowercase-global

function exec(...)
    local arguments = { ... }
    local cmd = ""
    for _, v in ipairs(arguments) do
        cmd = cmd .. ' ' .. v
    end
    print(os.exec(cmd))
end

function exec_cmds(...)
    local arguments = { ... }
    for _, v in ipairs(arguments) do
        print(os.exec(v))
    end
end
