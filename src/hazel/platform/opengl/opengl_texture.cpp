//
// Created by nono on 9/24/23.
//
#include "hz_pch.h"

#include "hazel/core/log.h"

#include "opengl_texture.h"
#include <glad/glad.h>

#include "gl_macros.h"
#include "stb/stb_image.h"

namespace hazel {
OpenGLTexture2D::OpenGLTexture2D(const std::string &path)
    : m_Path(path)
{
    HZ_PROFILE_FUNCTION();

    HZ_CORE_TRACE("Creating OpenGLTexture2D: {}", path);

    int      w, h, nChannel;
    stbi_uc *data = nullptr;
    {
        HZ_PROFILE_SCOPE("OpenGLTexture2D::OpenGLTexture2D(const std::string &path) [stbi_load_image]");
        stbi_set_flip_vertically_on_load(1);
        data = stbi_load(path.c_str(), &w, &h, &nChannel, 0);
    }

    if (!data) {
        HZ_CORE_WARN("Failed to load image: {}", path);
        return;
    }

    HZ_CORE_INFO("w: {}, h: {}, nChannel: {}", w, h, nChannel);
    m_IsLoaded = true;
    m_Width    = w;
    m_Height   = h;

    m_InternalFormat = nChannel == 3 ? GL_RGB8 : GL_RGBA8;
    m_DataFormat     = nChannel == 3 ? GL_RGB : GL_RGBA;
    HZ_CORE_ASSERT(m_InternalFormat & m_DataFormat, "NOT supported image format");

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
    HZ_PROFILE_FUNCTION();

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
    HZ_PROFILE_FUNCTION();

    // GL_CHECK_HEALTH();
    if (m_TextureID != 0) {
        HZ_CORE_TRACE("<Deleting the texture: {}, texture id: {}", m_Path, m_TextureID);
        // glBegin(GL_POINT);
        // glVertex2f(2, 2);
        // glEnd();
        glDeleteTextures(1, &m_TextureID);
        // HZ_CORE_INFO("Deleted the texture: {}, texture id: {}", m_Path, m_TextureID);
        // glFinish();
        m_TextureID = 0;
        HZ_CORE_TRACE(">Deleted the texture: {}, texture id: {}", m_Path, m_TextureID);
    }
}

void OpenGLTexture2D::Bind(uint32_t slot) const
{
    GL_CALL(glBindTextureUnit(slot, m_TextureID));
}
void OpenGLTexture2D::Unbind() const
{
    // TODO
    // glBindTexture(slot, m_TextureID);
}

void OpenGLTexture2D::SetData(void *data, uint32_t size)
{
    HZ_PROFILE_FUNCTION();

    uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
    HZ_CORE_ASSERT(size == m_Width * m_Height * bpp, "Error: data must be entire texture!");
    glTextureSubImage2D(m_TextureID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
}

} // namespace hazel