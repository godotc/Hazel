local TestModule = require("Module.Test.TestModule")


function MainEntry(...)
    print("MainEntry")
    local module = TestModule:New()
    module:Test()
end

pcall(MainEntry)
MainEntry()
