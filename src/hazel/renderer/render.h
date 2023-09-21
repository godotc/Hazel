//
// Created by nono on 9/20/23.
//

#ifndef HAZEL_RENDER_H
#define HAZEL_RENDER_H

#include "renderer_api.h"

namespace hazel {

class Render
{
  public:
    static void BeginScene();
    static void EndScene();

    //    static void Submit(std::shared_ptr<Shader> &vertex_array);
    static void Submit(std::shared_ptr<VertexArray> &vertex_array);

  public:
    static inline RendererAPI::API GetRendererAPI() { return RendererAPI::GetAPI(); }
};

} // namespace hazel

#endif // HAZEL_RENDER_H
