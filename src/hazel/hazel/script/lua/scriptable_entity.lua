function class(name, base)
    local cls = {}
    cls.__index = cls
    cls.__name = name
    cls.__base = base
    setmetatable(cls, base)
    return cls
end

Object = class("Object")

function Object:New()
    local obj = {}
    setmetatable(obj, self)
    self.__index = self
    return obj
end

local ScriptableEntity = class("ScriptableEntity")

function ScriptableEntity:OnCreate()
end

function ScriptableEntity:OnUpdate(dt)

end

function ScriptableEntity:OnDestroy(dt)

end

return ScriptableEntity
