#pragma once

#include "hazel/renderer/framebuffer.h"
#include <cstdint>
#include <functional>
#include <winbase.h>

namespace hazel {

class OpenGLFrameBuffer : public Framebuffer
{
    uint32_t        m_FramebufferID = 0;
    FramebufferSpec m_Specification;

    std::vector<framebuffer::TextureSpec> m_ColorAttachmentSpecs;
    framebuffer::TextureSpec              m_DepthAttachmentSpec = framebuffer::TextureSpec{framebuffer::ETextureFormat::None};
    std::vector<glm::uint32>              m_ColorAttachments;
    uint32_t                              m_DepthAttachment = 0;

  public:

    explicit OpenGLFrameBuffer(const FramebufferSpec &spec);
    ~OpenGLFrameBuffer() override;

    void Bind() override;
    void Unbind() override;
    void Resize(uint32_t w, uint32_t h) override;

    uint32_t GetColorAttachmentID(uint32_t index = 0) const override;
    void     ClearAttachment(uint32_t attachment_index, int value) override;
    int      ReadPixel(uint32_t attachment_index, int x, int y) const override;


    void UpdateAll();

    [[nodiscard]] const FramebufferSpec &GetSpecification() const override;
};

} // namespace hazel