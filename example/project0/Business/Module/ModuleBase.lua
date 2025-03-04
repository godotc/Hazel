require "Business.FrameWork.Object"

---@class ModuleBase: Object
local ModuleBase = class("ModuleBase", Object)

function ModuleBase:Constructor()
    self.Super().Constructor()
    print("ModuleBase:Constructor")
end

return ModuleBase
