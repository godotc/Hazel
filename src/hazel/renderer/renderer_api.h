//
// Created by nono on 9/22/23.
//
#pragma once

#include "vertex_array.h"
#include <glm/glm.hpp>


namespace hazel {

class RendererAPI
{
  public:
    enum class API
    {
        NONE   = 0,
        OPENGL = 1,
        VULKAN,
        DIRECTX,
        METAL
    };

  public:
    RendererAPI() = default;

    virtual void Init()                                = 0;
    virtual void SetClearColor(const glm::vec4 &color) = 0;
    virtual void Clear()                               = 0;

    virtual void DrawIndexed(const Ref<VertexArray> &vertex_array) = 0;

    static inline API GetAPI() { return s_API; }

  private:
    static API s_API;
};

} // namespace hazel
