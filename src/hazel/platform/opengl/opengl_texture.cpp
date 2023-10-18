//
// Created by nono on 9/24/23.
//
#include "hz_pch.h"

#include "opengl_texture.h"
#include <glad/glad.h>

#include "gl_macros.h"
#include "stb/stb_image.h"

namespace hazel {
OpenGLTexture2D::OpenGLTexture2D(const std::string &path)
    : m_Path(path)
{
    int w, h, nChannel;
    stbi_set_flip_vertically_on_load(1);
    stbi_uc *data = stbi_load(path.c_str(), &w, &h, &nChannel, 0);
    HZ_CORE_ASSERT(data, "Failed to load image!");

    m_Width  = w;
    m_Height = h;

    m_InternalFormat = nChannel == 3 ? GL_RGB8 : GL_RGBA8;
    m_DataFormat     = nChannel == 3 ? GL_RGB : GL_RGBA;

    HZ_CORE_ASSERT(m_InternalFormat & m_DataFormat, "NO supported image format");

    glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);
    glTextureStorage2D(m_TextureID, 1, m_InternalFormat, m_Width, m_Height);

    glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTextureSubImage2D(m_TextureID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
}

OpenGLTexture2D::OpenGLTexture2D(uint32_t w, uint32_t h)
{
    m_Width  = w;
    m_Height = h;

    glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);
    glTextureStorage2D(m_TextureID, 1, m_InternalFormat, m_Width, m_Height);

    glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

OpenGLTexture2D::~OpenGLTexture2D()
{
    glDeleteTextures(1, &m_TextureID);
}

void OpenGLTexture2D::Bind(uint32_t slot) const
{
    GL_CALL(glBindTextureUnit(slot, m_TextureID));
}
void OpenGLTexture2D::Unbind() const
{
    // TODO
    //     glBindTexture(slot, m_TextureID);
}

void OpenGLTexture2D::SetData(void *data, uint32_t size)
{
    uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
    HZ_CORE_ASSERT(size == m_Width * m_Height * bpp, "Error: data must be entire texture!");
    glTextureSubImage2D(m_TextureID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
}

} // namespace hazel