//
// Created by nono on 9/22/23.
//

#pragma once

#include "hazel/renderer/renderer_api.h"

namespace hazel {

class RenderCommand
{
  public:
    inline static void SetClearColor(const glm::vec4 &color) { Get()->SetClearColor(color); }
    inline static void Clear() { Get()->Clear(); }

    static inline void DrawIndex(const Ref<VertexArray> &vertexArray) { Get()->DrawIndexed(vertexArray); }

  private:
    static hazel::RendererAPI *Get();
};

} // namespace hazel
