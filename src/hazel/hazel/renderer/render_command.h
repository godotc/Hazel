//
// Created by nono on 9/22/23.
//

#pragma once

#include "hazel/renderer/renderer_api.h"
#include <cstdint>

namespace hazel {

class HAZEL_API RenderCommand
{
  public:
    inline static void Init() { Get()->Init(); }
    inline static void SetClearColor(const glm::vec4 &color) { Get()->SetClearColor(color); }
    inline static void Clear() { Get()->Clear(); }

    static inline void DrawIndex(const Ref<VertexArray> &vertexArray, uint32_t count = 0) { Get()->DrawIndexed(vertexArray, count); }

    static void SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h);

  private:
    static hazel::RendererAPI *Get();
};

} // namespace hazel
