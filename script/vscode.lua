local vscode = {}

local json = require "json"
local t = require("table")

-- Read the JSON file
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

-- Edit the JSON data
function edit_json_data(json_data)
end


function vscode.update_launch_profile()
    local file_path = ".vscode/launch.json" 
    -- Read JSON file
    local json_str = read_json_file(file_path)

    -- Parse JSON string into a Lua table
    local json_data = json.decode(json_str)

    print(json_data)

    -- -- Edit the JSON data
    -- local edited_data = edit_json_data(json_data)

    -- -- Write the modified JSON data back to the file
    -- write_json_file(file_path, edited_data)

    -- print("JSON file has been edited successfully.")

    -- -- Make your desired changes here. For example, let's add a new key-value pair:
    -- json_data["new_key"] = "new_value"

    -- return json_data
end
