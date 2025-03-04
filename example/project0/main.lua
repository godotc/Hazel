local original_traceback = debug.traceback

function new_traceback(msg, level)
    level = (level or 1) + 1 -- 跳过当前函数
    local tb = {}
    if msg then
        -- 确保消息为字符串
        if type(msg) ~= "string" and type(msg) ~= "number" then
            msg = tostring(msg)
        end
        table.insert(tb, msg)
    end
    table.insert(tb, "stack traceback:")

    local stack_level = level
    while true do
        local info = debug.getinfo(stack_level, "nSl")
        if not info then break end

        -- 处理源码路径
        local source = info.source
        if source:sub(1, 1) == "@" then
            source = source:sub(2)      -- 移除开头的 '@'
        end
        source = source:gsub("\\", "/") -- 统一为正斜杠

        -- 拼接工作目录
        if WORKING_DIR then
            source = WORKING_DIR .. "/" .. source
        end

        -- 构造行信息
        local line = string.format(
            "%s:%d: in %s '%s'",
            source,
            info.currentline or 0,
            info.what,
            info.name or "anonymous"
        )
        table.insert(tb, line)

        stack_level = stack_level + 1
    end

    return table.concat(tb, "\n")
end

debug.traceback = new_traceback

-- 设置 package.path
if WORKING_DIR then
    print("WORKING_DIR=", WORKING_DIR)
    package.path = package.path .. ";" .. WORKING_DIR .. "/Business/?.lua"
end
print(package.path)


print("wtf")
require("Business.MainEntry")
-- print(BusinessMain)
-- BusinessMain()

-- BusinessMain:b()

BusinessMain()
