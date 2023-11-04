#pragma once

#include "hazel/renderer/framebuffer.h"
#include <cstdint>
#include <functional>

namespace hazel {

class OpenGLFrameBuffer : public Framebuffer
{
    uint32_t                 m_FramebufferID;
    uint32_t                 m_ColorAttachment;
    uint32_t                 m_DepthAttachment;
    FramebufferSpecification m_Specification;

  public:

    OpenGLFrameBuffer(const FramebufferSpecification &spec);
    ~OpenGLFrameBuffer();

    void     Bind() override;
    void     Unbind() override;
    uint32_t GetColorAttachmentID() override;


    void Init();

    const FramebufferSpecification &GetSpecification() const override;
};

} // namespace hazel