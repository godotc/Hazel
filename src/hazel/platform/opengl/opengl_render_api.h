//
// Created by nono on 9/22/23.
//

#ifndef HAZEL_OPEN_GL_RENDER_API_H
#define HAZEL_OPEN_GL_RENDER_API_H

#include "hazel/renderer/renderer_api.h"

namespace hazel {

class OpenGLRenderAPI : public RendererAPI
{
  public:
    void Init() override;
    void SetClearColor(const glm::vec4 &color) override;
    void Clear() override;

    void SetViewPort(uint32_t x, uint32_t y, uint32_t w, uint32_t h) override;

    void DrawIndexed(const Ref<VertexArray> &vertex_array) override;
};

} // namespace hazel

#endif // HAZEL_OPEN_GL_RENDER_API_H
