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

    m_Width             = w;
    m_Height            = h;
    auto storage_format = nChannel == 3 ? GL_RGB8 : GL_RGBA8;
    auto image_format   = nChannel == 3 ? GL_RGB : GL_RGBA;

    glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);
    glTextureStorage2D(m_TextureID, 1, storage_format, m_Width, m_Height);

    glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTextureSubImage2D(m_TextureID, 0, 0, 0, m_Width, m_Height, image_format, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
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

} // namespace hazel