local ModuleBase = require "Business.Module.ModuleBase"


---@class TestModule: ModuleBase
local TestModule = class("TestModule", ModuleBase)


function TestModule:Constructor()
    print("TestModule:ctor")
    self.name = "TestModule"
end

function TestModule:Test()
    print("TestModule:Test")
end

return TestModule
