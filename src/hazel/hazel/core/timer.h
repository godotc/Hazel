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
    explicit Timer()
    {
        m_StartTimePoint = std::chrono::high_resolution_clock::now();
    }
    // TODO: Test the return values...
    float Elapsed()
    {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(m_StartTimePoint.time_since_epoch()).count();
    }

    long long ElapsedMillis()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(m_StartTimePoint.time_since_epoch()).count();
    }
};
} // namespace hazel
