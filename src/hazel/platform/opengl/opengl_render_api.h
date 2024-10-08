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
    void Clear() override;

    void SetClearColor(const glm::vec4 &color) override;
    void SetViewPort(uint32_t x, uint32_t y, uint32_t w, uint32_t h) override;
    void SetLineWidth(float width) override;

    void DrawIndexed(const Ref<VertexArray> &vertex_array, uint32_t index_count) override;
    void DrawLines(const Ref<VertexArray> &vertex_array, uint32_t vertex_count) override;
};

} // namespace hazel

#endif // HAZEL_OPEN_GL_RENDER_API_H
