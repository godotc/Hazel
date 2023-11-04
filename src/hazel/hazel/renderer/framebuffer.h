#pragma once
#include "hazel/core/base.h"

#include <cstdint>

namespace hazel {

struct FramebufferSpecification {
    uint32_t Width, Height;

    uint32_t Samples = 1;

    bool bSwapChainTarget = false;
};

class Framebuffer
{

  public:
    virtual const FramebufferSpecification &GetSpecification() const = 0;

    static Ref<Framebuffer> Create(const FramebufferSpecification &spec);
};


} // namespace hazel