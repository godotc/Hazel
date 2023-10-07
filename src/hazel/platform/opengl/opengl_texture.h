//
// Created by nono on 9/24/23.
//

#ifndef HAZEL_OPENGL_TEXTURE_H
#define HAZEL_OPENGL_TEXTURE_H

#include "hazel/renderer/texture.h"

namespace hazel {

class OpenGLTexture2D : public Texture2D
{
  public:
    OpenGLTexture2D(const std::string &path);
    ~OpenGLTexture2D() override;

  public:
    void Bind(uint32_t slot) const override;
    void Unbind() const override;

    uint32_t GetWidth() const override { return m_Width; };
    uint32_t GetHeight() const override { return m_Height; }

  private:
    std::string m_Path;
    uint32_t    m_Width, m_Height;
    uint32_t    m_TextureID = {0};
};

} // namespace hazel

#endif // HAZEL_OPENGL_TEXTURE_H
