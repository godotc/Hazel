---@diagnostic disable: lowercase-global


---@class CommonConfig : table
---@field public boolean

---@param ... string| "module.debug" | "module.release"
function add_rules(...) end

do
    ---@param ... string
    function add_packages(...) end

    ---@param name string
    ---@param cfg table
    function add_packages(name, cfg) end
end

do
    ---@param ... string
    function add_requires(...) end

    ---@param name string
    ---@param cfg CommonConfig
    function add_requires(name, cfg) end
end


---@overload fun( ...:string)
---@overload fun(pkg: string, cfg: CommonConfig)
function add_deps() end

---@param ... string
function add_files(...) end

---@param ... string
function add_headerfiles(...) end

---@param ... string
function add_includedirs(...) end

---@class PackageConfig : table
---@field config table

---@param package string
---@param config PackageConfig
function add_requires(package, config) end

---@param type "binary"|"shared"|"static"
function set_kind(type) end

---@param lang "c++20"|"c++17"|"c++14"|"c++11"|"c++0x"|"c++98"
function set_languages(lang) end

---@param dir string
function set_targetdir(dir) end

---@param app string
function target(app) end

---@param v "cxx"|"no-cxx"
function set_exceptions(v) end

---@param ... string
function add_cxflags(...) end

---@param ... string
function add_cxxflags(...) end

---@param platform "windows"|"linux"|"ios"
function is_plat(platform) end

---@param ... string
function includes(...) end
