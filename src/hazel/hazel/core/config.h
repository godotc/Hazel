/**
 *  Author: @godot42
 *  Create Time: 2024-08-02 01:51:41
 *  Modified by: @godot42
 *  Modified time: 2024-08-02 01:52:21
 *  Description:
 */



#pragma once

#include "base.h"
#include "log.h"
#include "utils/file.h"
#include "utils/path.h"
#include "utils/trait.h"
#include <string>

#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>


namespace hazel {


struct DefaultConfig : public utils::trait::disable_copy {


  public:
    void Init()
    {
        auto filepath = FPath("config") / ConfigName();

        auto content = utils::File::read_all(filepath);
        if (!content.has_value()) {
            HZ_CORE_ERROR("Failed to read config file: {}", filepath.string());
        }

        auto config = nlohmann::json::parse(filepath);

        // TODO: reflect all variable in content
    }
    constexpr std::string ConfigName() { return "DefaultConfig"; }
};

} // namespace hazel