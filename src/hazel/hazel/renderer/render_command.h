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
    static void Init() { GetRenderAPI()->Init(); }

    static void Clear() { GetRenderAPI()->Clear(); }

    static void SetClearColor(const glm::vec4 &color) { GetRenderAPI()->SetClearColor(color); }
    static void SetLineWidth(float width) { GetRenderAPI()->SetLineWidth(width); }
    static void SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h) { GetRenderAPI()->SetViewPort(x, y, w, h); }

    static void DrawIndex(const Ref<VertexArray> &vertexArray, uint32_t count = 0) { GetRenderAPI()->DrawIndexed(vertexArray, count); }
    static void DrawLines(const Ref<VertexArray> &vertexArray, uint32_t count = 0) { GetRenderAPI()->DrawLines(vertexArray, count); }

  private:
    static hazel::RendererAPI *GetRenderAPI();
};



} // namespace hazel
