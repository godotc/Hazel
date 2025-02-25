/**
 * @ Author: godot42
 * @ Create Time: 2024-08-18 00:58:12
 * @ Modified by: @godot42
 * @ Modified time: 2024-08-18 01:18:48
 * @ Description:
 */



#include "hz_pch.h"
//

#include "uuid.h"
#include <random>

namespace hazel {


static std::random_device                      s_RandomDevice;
static std::mt19937_64                         s_Engine(s_RandomDevice());
static std::uniform_int_distribution<uint64_t> s_UniformDistribution;

// TODO: a hash map to validate the uniqueness of UUIDs


UUID::UUID() { m_UUID = s_UniformDistribution(s_Engine); }

UUID::UUID(uint64_t uuid) : m_UUID(uuid) {}



} // namespace hazel
