/**
 *  Author: @godot42
 *  Create Time: 2024-03-15 00:22:11
 *  Modified by: @godot42
 *  Modified time: 2024-03-15 01:33:08
 *  Description:
 */

#pragma once

#include <chrono>

namespace hazel {

class Timer
{
    std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimePoint;

  public:
    Timer()
    {
        m_StartTimePoint = {std::move(std::chrono::high_resolution_clock::now())};
    }

    // NOTICE: time_since_epoch will return the time since 1970-01-01 00:00:00

    long long Elapsed()
    {
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(now - m_StartTimePoint).count();
    }

    long long ElapsedMillis()
    {
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(now - m_StartTimePoint).count();
        // return std::chrono::duration_cast<std::chrono::milliseconds>(m_StartTimePoint.time_since_epoch()).count();
    }
};
} // namespace hazel
