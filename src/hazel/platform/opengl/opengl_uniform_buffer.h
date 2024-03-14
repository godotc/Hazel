/**
 *  Author: @godot42
 *  Create Time: 2024-03-15 00:52:26
 *  Modified by: @godot42
 *  Modified time: 2024-03-15 00:58:22
 *  Description:
 */

#pragma once

#include "hazel/renderer/uniform_buffer.h"
namespace hazel {


class OpenGLUniformBuffer : public UniformBuffer
{
  public:
    OpenGLUniformBuffer(uint32_t size, uint32_t binding);
    virtual ~OpenGLUniformBuffer();

    virtual void SetData(const void *data, uint32_t size, uint32_t offset = 0) override;

  private:
    uint32_t m_UniformBufferID = 0;
};

} // namespace hazel
