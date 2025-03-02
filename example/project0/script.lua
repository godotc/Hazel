local Script = {}

function Script:New()
    local obj = {}
    setmetatable(obj, self)
    self.__index = self
    return obj
end

function Script:OnCreate()
    print("Script:OnCreate")
end

function Script:OnUpdate(dt)
    print("Script:OnUpdate", dt)
end

function Script:OnDestroy()
    print("Script:OnDestroy")
end

return Script
