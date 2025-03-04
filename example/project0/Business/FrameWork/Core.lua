---@param name string
---@generic T : ClassBase
---@param parent T|nil
function class(name, parent)
    print("Add class", name, parent and parent._name)
    ---@class ClassBase
    local cls = {}
    cls._name = name
    cls._parent = parent

    local mt = setmetatable(cls, {
        __index = function(t, k)
            print("__index", t, k)
            local v = rawget(t, k)
            if v then return v end

            local child = t
            local parent = nil
            while true do
                parent = rawget(child, "_parent")
                if not parent then
                    return nil
                end
                print("iterate parent", parent, parent._name)

                v = rawget(parent, k)
                if v then return v end

                child = parent
            end
        end
    }
    )


    return cls
end
