local json = require("script/dkjson")

require("script.utils")


local function read_json_file(file_path)
    local file, err = io.open(file_path, "r")
    if not file then
        error("Cannot open file " .. file_path .. ": " .. err)
    end
    local json_str = file:read("*all")
    file:close()
    return json_str
end


-- Write the modified JSON data back to the file
local function write_json_file(file_path, json_data)
    local file, err = io.open(file_path, "w")
    if not file then
        error("Cannot open file " .. file_path .. ": " .. err)
    end

    local json_str = json.encode(json_data, {
        indent = true
    })
    file:write(json_str)
    file:close()
end


local function gen_lldb_configurations(configs)
    local ret = {}

    local tbl = {
        name = "",
        type = "lldb",
        request = "launch",
        args = {},
        cwd = "${workspaceFolder}",
        windows = {
            program = ""
        },
        preLaunchTask = "build " .. configs.target_name,
    }

    local name = string.format("[%s] LLDB ", configs.target_name)

    -- debug
    tbl.name = name .. "DEBUG"
    tbl.windows.program =
        string.format("${workspaceFolder}/%s/%s/%s", configs.target_dir, "debug", configs.target_base_name)
    table.insert(ret, tbl)

    -- relase
    tbl = Utils.DeepCopy(tbl)
    tbl.name = name .. "RELEASE"
    tbl.windows.program =
        string.format("${workspaceFolder}/%s/%s/%s", configs.target_dir, "release", configs.target_base_name)
    table.insert(ret, tbl)

    return ret
end


local function gen_gdb_configurations(configs)
    local ret = {}

    local tbl = {
        name = "",
        type = "gdb",
        request = "launch",
        arguments = "",
        cwd = "${workspaceFolder}",
        windows = {
            target = "",
        },
        -- linux = {
        --     target = ""
        -- },
        valuesFormatting = "prettyPrinters",
        internalConsoleOptions = "openOnSessionStart",
        preLaunchTask = "build " .. configs.target_name,
    }

    local name = string.format("[%s] GDB ", configs.target_name)

    -- debug
    tbl.name = name .. "DEBUG"
    tbl.windows.target =
        string.format("${workspaceFolder}/%s/%s/%s", configs.target_dir, "debug", configs.target_base_name)
    table.insert(ret, tbl)

    -- relase
    tbl = Utils.DeepCopy(tbl)
    tbl.name = name .. "RELEASE"
    tbl.windows.target =
        string.format("${workspaceFolder}/%s/%s/%s", configs.target_dir, "relase", configs.target_base_name)
    table.insert(ret, tbl)

    return ret
end



local function update_launch_profile(target_name, target_dir, target_base_name)
    local file_path = ".vscode/launch.json"
    -- Read JSON file
    local json_str = read_json_file(file_path)
    -- print(json_str)
    -- local pattern = "%s-//[^\r\n]*" -- Matches comments starting with // and ending with a newline character
    -- json_str = string.gsub(json_str, pattern, "")
    -- print(json_str)

    -- Parse JSON string into a Lua table
    local json_data = json.decode(json_str)
    -- print(json_data)
    -- print(Utils.Dump(json_data))

    local configs = {
        target_name = target_name,
        target_dir = target_dir,
        target_base_name = target_base_name,
    }

    assert(json_data, " decode raw file failed!")
    -- json_data  = json_data or {}

    -- Utils.DumpPrint(generates)

    local function addd_or_replace_configurations(configurations, new_configuration)
        for _, v in ipairs(configurations) do
            if v.name == new_configuration.name then
                v = new_configuration
                print("replace launch config: " .. new_configuration.name)
                return
            end
        end
        table.insert(configurations, new_configuration)
        print("add launch config: " .. new_configuration.name)
    end

    local generates = gen_lldb_configurations(configs)
    for _, v in ipairs(generates) do
        addd_or_replace_configurations(json_data.configurations, v)
    end

    generates = gen_gdb_configurations(configs)
    for _, v in ipairs(generates) do
        addd_or_replace_configurations(json_data.configurations, v)
    end

    -- Utils.DumpPrint(json_data)

    write_json_file(file_path, json_data)
end



local function update_task_profile(target_name)
    local file_path = ".vscode/tasks.json"
    local json_str = read_json_file(file_path)
    local json_data = json.decode(json_str)
    assert(json_data, " decode raw file failed!")

    local tbl = {
        label = "",
        type = "shell",
        command = "xmake",
        args = {
            "build",
            "",
        },
        presentation = {
            echo = true,
            reveal = "always",
            focus = false,
            panel = "shared",
            showReuseMessage = true,
            clear = false
        }
    }
    tbl.label = string.format("build %s", target_name)
    tbl.args[2] = target_name

    local bReplaced = false

    for _, v in ipairs(json_data.tasks) do
        if v.label == tbl.label then
            v = tbl;
            print("replace task: " .. tbl.label)
            bReplaced = true;
        end
    end
    if not bReplaced then
        table.insert(json_data.tasks, tbl)
        print("add task: " .. tbl.label)
    end


    -- Utils.DumpPrint(json_data)

    write_json_file(file_path, json_data)
end



local target_name = arg[1] or "__DEFAULT_VAR__"
local target_dir = arg[2] or "__DEFAULT_VAR__"
local target_base_name = arg[3] or "__DEFAULT_VAR__"

-- print(arg)
-- Utils.DumpPrint(arg)

update_task_profile(target_name) -- build target_name
update_launch_profile(target_name, target_dir, target_base_name)
