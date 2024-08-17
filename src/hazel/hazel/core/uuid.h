/**
 * @ Author: godot42
 * @ Create Time: 2024-08-18 00:58:03
 * @ Modified by: @godot42
 * @ Modified time: 2024-08-18 01:04:49
 * @ Description:
 */



#pragma once

#include <cstdint>
#include <xhash>

namespace hazel {

class UUID
{
    uint64_t m_UUID;

  public:
    UUID();
    UUID(uint64_t uuid);
    UUID(const UUID &other) = default;

    operator uint64_t() const { return m_UUID; }
};

} // namespace hazel



namespace std {
template <>
struct hash<hazel::UUID> {
    std::size_t operator()(const hazel::UUID &uuid) const
    {
        return hash<uint64_t>()(uuid);
    }
};
} // namespace std