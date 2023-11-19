//
// Created by nono on 9/18/23.
//

#pragma once

#include "hazel/core/base.h"

namespace hazel {

class GraphicsContext
{
  public:
    GraphicsContext()          = default;
    virtual ~GraphicsContext() = default;

    virtual void Init()        = 0;
    virtual void SwapBuffers() = 0;
};

} // namespace hazel
