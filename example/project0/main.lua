-- debug config
local original_traceback = debug.traceback

debug.traceback = function(msg, level)
    level = (level or 1) + 1 -- 默认层级+1，跳过当前函数
    local tb = original_traceback(msg, level)
    print(123)
    tb = tb:gsub("(%.%.%).-([%w_]+%.lua)", function(_, filename)
        -- 查找文件名对应的完整路径
        local stack_level = 2 -- 从调用栈的上一层开始
        while true do
            local info = debug.getinfo(stack_level, "S")
            if not info then break end
            local path = info.source:sub(2)
            if path:find(filename, 1, true) then
                return path:gsub("\\", "/")
            end
            stack_level = stack_level + 1
        end
        return "unknown_path/" .. filename
    end)
    return tb
end


print(WORKING_DIR)
if WORKING_DIR ~= nil then
    package.path = package.path .. ";" .. WORKING_DIR .. "/Business/?.lua"
end


require "Business.MainEntry"

xpcall(MainEntry, print)
