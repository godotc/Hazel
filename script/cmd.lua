function exec(...)
    local arguments = { ... }
    local cmd = ""
    for _, v in ipairs(arguments) do
        cmd = cmd .. ' '..v 
    end
    print(os.exec(cmd))
end
