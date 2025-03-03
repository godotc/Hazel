---@param name string
---@generic T : ClassBase
---@param parent T|nil
function class(name, parent)
    ---@class ClassBase
    local cls = {}
    cls._name = name
    if parent then
        cls._parent = parent
    end
    return cls
end

---@class Object:ClassBase
Object = class("Object")

function Object:New()
    ---@class Object
    local obj = {}
    setmetatable(obj, self)
    self.__index = self
    obj:Constructor()
    return obj
end

function Object:super()
    return self._parent
end

function Object:Constructor()
    -- self.super():Constructor()
    print("Object:Constructor")
end

---@class ModuleBase: Object
local ModuleBase = class("ModuleBase", Object)

function ModuleBase:Constructor()
    self.super().Constructor()
    print("ModuleBase:Constructor")
end

return ModuleBase
