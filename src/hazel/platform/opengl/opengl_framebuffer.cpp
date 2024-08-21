#include "hz_pch.h"

#include "hazel/core/base.h"
#include "hazel/renderer/framebuffer.h"
#include "hazel/renderer/render_command.h"

#include "glad/glad.h"

#include "opengl_framebuffer.h"

#include "gl_macros.h"



namespace hazel {

static const uint32_t s_MaxFrameBUfferSize = 8192;

namespace utils {
static GLenum TextureTarget(const bool bMultiSample) { return bMultiSample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D; }
static void   CreateTextures(const bool bMultiSample, uint32_t *out_id_ptr, const uint32_t count)
{
    glCreateTextures(TextureTarget(bMultiSample), static_cast<GLsizei>(count), out_id_ptr);
}
static void BindTexture(const bool bMultiSample, const uint32_t texture_id)
{
    glBindTexture(TextureTarget(bMultiSample), texture_id);
}
static auto AttachColorTexture(const uint32_t texture_id, const int samples,
                               const GLenum internal_format, const GLenum format,
                               const uint32_t w, const uint32_t h, const int index) -> void
{
    const bool bMultiSample = samples > 1;
    if (bMultiSample) {
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internal_format, w, h, GL_FALSE);
    }
    else {
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, w, h, 0, format, GL_UNSIGNED_BYTE, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // TODO: Filtering
    }
    // Finally attach it
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index,
                           TextureTarget(bMultiSample), texture_id, 0);
}

static auto AttachDepthTexture(const uint32_t texture_id, const int samples,
                               const GLenum format, GLenum attachment_type,
                               const uint32_t w, const uint32_t h) -> void
{
    const bool bMultiSample = samples > 1;
    if (bMultiSample) {
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, w, h, GL_FALSE);
    }
    else {
        glTexStorage2D(GL_TEXTURE_2D, 1, format, w, h);

        // NOTICE: deferent of the glTexParameter and glTextureParamater freaking same old issue!!!!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        GL_CHECK_HEALTH();

        // TODO: Filtering
    }
    // Finally attach it
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_type,
                           TextureTarget(bMultiSample), texture_id, 0);
    GL_CHECK_HEALTH();
}


static bool IsDepthFormat(const framebuffer::ETextureFormat format)
{
    switch (format) {
        case framebuffer::ETextureFormat::DEPTH24_STENCIL8:
            // case framebuffer::ETextureFormat::Depth:
            return true;
        default:;
    }
    return false;
}

static GLenum HazelFBTextureFormatToGL(framebuffer::ETextureFormat format)
{
    switch (format)
    {
        case framebuffer::ETextureFormat::RGBA8:
            return GL_RGBA8;
        case framebuffer::ETextureFormat::RED_INTEGER:
            return GL_RED_INTEGER;
        case framebuffer::ETextureFormat::None:
        case framebuffer::ETextureFormat::RGB16F:
        case framebuffer::ETextureFormat::DEPTH24_STENCIL8:
            break;
    }

    HZ_CORE_ASSERT(false);
    return 0;
}
} // namespace utils

OpenGLFrameBuffer::OpenGLFrameBuffer(const FramebufferSpec &framebuffer_spec)
{
    m_Specification = framebuffer_spec;
    for (const auto &spec : m_Specification.Attachments.Attachments)
    {
        if (!utils::IsDepthFormat(spec.TextureFormat)) {
            m_ColorAttachmentSpecs.emplace_back(spec);
        }
        else {
            m_DepthAttachmentSpec = spec;
        }
    }
    UpdateAll();
}

OpenGLFrameBuffer::~OpenGLFrameBuffer()
{
    glDeleteFramebuffers(1, &m_FramebufferID);
    glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
    glDeleteTextures(1, &m_DepthAttachment);
}

void OpenGLFrameBuffer::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);

    // No need to calc the asp and set viewport, because see the ThisClass::UpdateProject(), it will make a projection matrix to do what you need
    // float asp = m_Specification.Width / (float)m_Specification.Height;
    // RenderCommand::SetViewport(0, 0, m_Specification.Width * asp, m_Specification.Height / asp);
    // RenderCommand::SetViewport(0, 0, m_Specification.Width, m_Specification.Height);
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

uint32_t OpenGLFrameBuffer::GetColorAttachmentID(uint32_t index) const
{
    HZ_CORE_ASSERT(index < m_ColorAttachments.size(), "Invalid ColorAttachment Index!");
    return m_ColorAttachments[index];
}

void OpenGLFrameBuffer::ClearAttachment(uint32_t attachment_index, int value)
{
    HZ_CORE_ASSERT(attachment_index < m_ColorAttachments.size(), "Invalid Attachment index");
    auto &spec = m_ColorAttachmentSpecs[attachment_index];
    glClearTexImage(m_ColorAttachments[attachment_index], 0,
                    utils::HazelFBTextureFormatToGL(spec.TextureFormat),
                    GL_INT, &value);
}

int OpenGLFrameBuffer::ReadPixel(uint32_t attachment_index, int x, int y) const
{
    HZ_CORE_ASSERT(attachment_index < m_ColorAttachments.size(), "Invalid Attachment index");
    glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment_index);
    int pixel = -1;
    glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, (void *)&pixel);
    // glReadPixels
    return pixel;
}


void OpenGLFrameBuffer::UpdateAll()
{
    if (!m_FramebufferID) {
        glDeleteFramebuffers(1, &m_FramebufferID);
        glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
        glDeleteTextures(1, &m_DepthAttachment);

        m_ColorAttachments.clear();
        m_DepthAttachment = 0;
    }


    glCreateFramebuffers(1, &m_FramebufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);

    bool bMultiSample = m_Specification.Samples > 1;

    // color attachments, different layers by the shaders's out location 0,1,2,3,4
    if (!m_ColorAttachmentSpecs.empty())
    {
        m_ColorAttachments.resize(m_ColorAttachmentSpecs.size());
        utils::CreateTextures(bMultiSample, m_ColorAttachments.data(), m_ColorAttachments.size());
        for (int i = 0; i < m_ColorAttachments.size(); ++i)
        {
            utils::BindTexture(bMultiSample, m_ColorAttachments[i]);
            switch (m_ColorAttachmentSpecs[i].TextureFormat)
            {
                case framebuffer::ETextureFormat::RGBA8:
                {
                    utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples,
                                              GL_RGBA8, GL_RGBA,
                                              m_Specification.Width, m_Specification.Height, i);
                    break;
                }
                case framebuffer::ETextureFormat::RED_INTEGER:
                {
                    utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples,
                                              GL_R32I, GL_RED_INTEGER,
                                              m_Specification.Width, m_Specification.Height, i);
                    break;
                }
                case framebuffer::ETextureFormat::None:
                case framebuffer::ETextureFormat::RGB16F:
                case framebuffer::ETextureFormat::DEPTH24_STENCIL8:
                    break;
            }
        }
    }
    GL_CHECK_HEALTH();

    // depth attachments
    if (m_DepthAttachmentSpec.TextureFormat != framebuffer::ETextureFormat::None)
    {
        utils::CreateTextures(bMultiSample, &m_DepthAttachment, 1);
        utils::BindTexture(bMultiSample, m_DepthAttachment);
        switch (m_DepthAttachmentSpec.TextureFormat) {
            case framebuffer::ETextureFormat::DEPTH24_STENCIL8:
            {
                utils::AttachDepthTexture(m_DepthAttachment, m_Specification.Samples,
                                          GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT,
                                          m_Specification.Width, m_Specification.Height);
                break;
            }
            case framebuffer::ETextureFormat::None:
            case framebuffer::ETextureFormat::RGBA8:
            case framebuffer::ETextureFormat::RED_INTEGER:
            case framebuffer::ETextureFormat::RGB16F:
                break;
        }
    }
    // GL_CHECK_HEALTH();

    if (m_ColorAttachments.size() > 1) {
        HZ_CORE_ASSERT(m_ColorAttachments.size() <= 4);
        std::array<GLenum, 4> buffers = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT0 + 1, GL_COLOR_ATTACHMENT0 + 2, GL_COLOR_ATTACHMENT0 + 3};
        glDrawBuffers(m_ColorAttachments.size(), buffers.data());
    }
    else if (m_ColorAttachments.empty()) {
        // only depth pass
        glDrawBuffer(GL_NONE);
    }

    // GL_CHECK_HEALTH();

#ifndef NDEBUG
    GLenum stats = glCheckFramebufferStatus(GL_FRAMEBUFFER);
#endif
    HZ_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

const FramebufferSpec &OpenGLFrameBuffer::GetSpecification() const
{
    return m_Specification;
}

} // namespace hazel
