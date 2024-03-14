/**
 *  Author: @godot42
 *  Create Time: 2024-03-15 00:38:28
 *  Modified by: @godot42
 *  Modified time: 2024-03-15 00:50:34
 *  Description:
 */


#pragma once

#include "hazel/core/base.h"
#include <cstdint>

namespace hazel {

class UniformBuffer
{
  public:
    virtual ~UniformBuffer() {}
    virtual void SetData(const void *data, uint32_t size, uint32_t offset = 0) = 0;

    static Ref<UniformBuffer> Create(uint32_t size, uint32_t binding);
};

} // namespace hazel