local TestModule = require("Module.Test.TestModule")

print("MainEntry")


function BusinessMain(...)
    print("MainEntry")
    local module = TestModule:New()
    module:Test()
end
