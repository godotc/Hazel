//
// Created by nono on 9/24/23.
//

#ifndef HAZEL_OPENGL_TEXTURE_H
#define HAZEL_OPENGL_TEXTURE_H

#include "glad/glad.h"
#include "hazel/renderer/texture.h"
#include <cstdint>

namespace hazel {

class OpenGLTexture2D : public Texture2D
{
    std::string m_Path;
    uint32_t    m_Width, m_Height;
    uint32_t    m_TextureID      = {0};
    GLenum      m_InternalFormat = {GL_RGBA8};
    GLenum      m_DataFormat     = {GL_RGBA};

  public:
    OpenGLTexture2D(uint32_t w, uint32_t h);
    OpenGLTexture2D(const std::string &path);
    ~OpenGLTexture2D() override;


  public:
    void Bind(uint32_t slot) const override;
    void Unbind() const override;

    void SetData(void *data, uint32_t size) override;

    uint32_t GetWidth() const override { return m_Width; };
    uint32_t GetHeight() const override { return m_Height; }

    uint32_t GetTextureID() const override { return m_TextureID; }


    virtual bool operator==(const Texture &other) const override
    {
        return m_TextureID == other.GetTextureID();
    }
};

} // namespace hazel

#endif // HAZEL_OPENGL_TEXTURE_H
