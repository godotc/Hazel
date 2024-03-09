#pragma once
#include "__microshit_api_hazel.h"
#include "hazel/core/base.h"

#include <cstdint>
#include <glm/fwd.hpp>
#include <initializer_list>
#include <vector>

namespace hazel {
namespace framebuffer {

enum class ETextureFormat
{
    None = 0,

    // color
    RGBA8,
    RED_INTEGER,

    RGB16F,

    // Depth/stencil
    DEPTH24_STENCIL8,

    // Defaults
    Depth = DEPTH24_STENCIL8,
};

struct TextureSpec {
    ETextureFormat TextureFormat;
    // TODO: filtering/wrap

    TextureSpec() = default;
    TextureSpec(ETextureFormat format)
        : TextureFormat(format){};
};

// Why attachment involove attachment? I don't like it
struct AttachmentSpec {
    std::vector<TextureSpec> Attachments;

    AttachmentSpec() = default;
    AttachmentSpec(const std::initializer_list<TextureSpec> attachments)
        : Attachments(attachments) {}
};

}; // namespace framebuffer

struct HAZEL_API FramebufferSpec {
    glm::uint32_t Width  = 800,
                  Height = 600;
    framebuffer::AttachmentSpec Attachments;
    uint32_t                    Samples = 1;

    bool bSwapChainTarget = false;
};

class HAZEL_API Framebuffer
{
  public:
    static Ref<Framebuffer> Create(const FramebufferSpec &spec);

    virtual ~Framebuffer() = default;

    virtual const FramebufferSpec &GetSpecification() const                                 = 0;
    virtual uint32_t               GetColorAttachmentID(uint32_t index = 0) const           = 0;
    virtual void                   ClearAttachment(uint32_t attachment_index, int value)    = 0;
    virtual int                    ReadPixel(uint32_t attachment_index, int x, int y) const = 0;

    virtual void Bind()   = 0;
    virtual void Unbind() = 0;

    virtual void Resize(uint32_t w, uint32_t h) = 0;
};


} // namespace hazel
