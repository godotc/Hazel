require "Business.FrameWork.Core"

---@class Object:ClassBase
Object = class("Object")

function Object:New()
    ---@type Object
    local obj = {}
    setmetatable(obj, self)
    self.__index = self
    obj:Constructor()
    return obj
end

function Object:Super()
    return self._parent
end

function Object:Constructor()
    -- self.super():Constructor()
    print("Object:Constructor")
end

return Object
