require("script.utils")

local function test_table_copy()
    local tbl = {}
    local a = { 1, 2 }
    table.insert(tbl, a)
    print("tbl: " .. Utils.Dump(tbl))
    print()

    local assign_a = a
    table.insert(assign_a, 3)
    print("a: " .. Utils.Dump(a))
    print("assign_a: " .. Utils.Dump(assign_a))
    print("tbl: " .. Utils.Dump(tbl))

    print()

    local deep_copy_a = Utils.DeepCopy(a)
    table.insert(deep_copy_a, 3)
    print("a: " .. Utils.Dump(a))
    print("deep_copy_a: " .. Utils.Dump(deep_copy_a))
    print("tbl: " .. Utils.Dump(tbl))

    print()
    table.insert(tbl, deep_copy_a)
    print("tbl: " .. Utils.Dump(tbl))
end


local function test_ref()
    local function f(n)
        n = n + 1
    end
    local n = 1
    f(n) -- did not chagne
    print(n)
end

local function test_array()
    local function f(a)
        print(type(a))
    end

    local a = { 1, 2, 3 }
    f(a)
    f({ 1, 2, 3 })
end


local function test_vargs()
    local function f(str, ...)
        return str .. ... -- only append once
    end

    print(f("hello", 1, 2, 3))
end

-- test_table_copy()
-- test_ref()
-- test_array()
test_vargs()
