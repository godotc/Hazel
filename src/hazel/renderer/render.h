//
// Created by nono on 9/20/23.
//

#ifndef HAZEL_RENDER_H
#define HAZEL_RENDER_H

namespace hazel {

enum class RendererAPI
{
    NONE   = 0,
    OPENGL = 1,
    VULKAN,
    DIRECTX,
    METAL
};

class Render
{
  public:
    static inline RendererAPI GetRendererAPI() { return API; }

  private:
    static RendererAPI API;
};

} // namespace hazel

#endif // HAZEL_RENDER_H
