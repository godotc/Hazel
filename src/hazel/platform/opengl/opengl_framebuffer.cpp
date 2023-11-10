#include "hazel/core/base.h"
#include "hz_pch.h"

#include "glad/glad.h"

#include "opengl_framebuffer.h"
#include <cstddef>

#

namespace hazel {

static const uint32_t s_MaxFrameBUfferSize = 8192;

OpenGLFrameBuffer::OpenGLFrameBuffer(const FramebufferSpecification &spec)
{
    m_Specification = spec;
    UpdateAll();
}

OpenGLFrameBuffer::~OpenGLFrameBuffer()
{
    glDeleteFramebuffers(1, &m_FramebufferID);
    glDeleteTextures(1, &m_ColorAttachment);
    glDeleteTextures(1, &m_DepthAttachment);
}

void OpenGLFrameBuffer::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
    glViewport(0, 0, m_Specification.Width, m_Specification.Height);
}

void OpenGLFrameBuffer::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFrameBuffer::Resize(uint32_t w, uint32_t h)
{
    if (w == 0 || h == 0 || w > s_MaxFrameBUfferSize || h > s_MaxFrameBUfferSize) {
        HZ_CORE_WARN("Window resized out of range {} {}", w, h);
        return;
    }


    m_Specification.Width  = w;
    m_Specification.Height = h;
    UpdateAll();
}

uint32_t OpenGLFrameBuffer::GetColorAttachmentID()
{
    return m_ColorAttachment;
}

void OpenGLFrameBuffer::UpdateAll()
{
    if (!m_FramebufferID) {
        glDeleteFramebuffers(1, &m_FramebufferID);
        glDeleteTextures(1, &m_ColorAttachment);
        glDeleteTextures(1, &m_DepthAttachment);
    }

    glCreateFramebuffers(1, &m_FramebufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
    {
        {
            glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachment);
            glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Specification.Width, m_Specification.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glTextureParameteri(m_ColorAttachment, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(m_ColorAttachment, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);
        }
        {
            glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);
            glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
            // TODO: something wrong
            glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height);
            // glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);
        }
    }
    HZ_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

const FramebufferSpecification &OpenGLFrameBuffer::GetSpecification() const
{
    return m_Specification;
}

} // namespace hazel
