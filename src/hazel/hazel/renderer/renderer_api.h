//
// Created by nono on 9/22/23.
//
#pragma once

#include "hazel/__microshit_api_hazel.h"

#include "vertex_array.h"
#include <cstdint>
#include <glm/glm.hpp>


namespace hazel {

class HAZEL_API RendererAPI
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

    virtual void Init()                                                      = 0;
    virtual void SetViewPort(uint32_t x, uint32_t y, uint32_t w, uint32_t h) = 0;
    virtual void SetClearColor(const glm::vec4 &color)                       = 0;
    virtual void Clear()                                                     = 0;

    virtual void DrawIndexed(const Ref<VertexArray> &vertex_array, uint32_t index_count) = 0;

    static inline API GetAPI() { return s_API; }

  private:
    static API s_API;
};

} // namespace hazel
