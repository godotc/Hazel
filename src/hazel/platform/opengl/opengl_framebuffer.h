#pragma once

#include "hazel/renderer/framebuffer.h"
#include <cstdint>
#include <functional>

namespace hazel {

class OpenGLFrameBuffer : public Framebuffer
{
    uint32_t                 m_FramebufferID   = 0;
    uint32_t                 m_ColorAttachment = 0;
    uint32_t                 m_DepthAttachment = 0;
    FramebufferSpecification m_Specification;

  public:

    OpenGLFrameBuffer(const FramebufferSpecification &spec);
    ~OpenGLFrameBuffer();

    void Bind() override;
    void Unbind() override;
    void Resize(uint32_t w, uint32_t h) override;

    uint32_t GetColorAttachmentID() override;


    void UpdateAll();

    const FramebufferSpecification &GetSpecification() const override;
};

} // namespace hazel