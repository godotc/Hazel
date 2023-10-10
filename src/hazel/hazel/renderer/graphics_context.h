//
// Created by nono on 9/18/23.
//

#pragma once

#include "hazel/hazel/core/core.h"

namespace hazel {

class GraphicsContext
{
  public:
    GraphicsContext() {}

    virtual void Init()        = 0;
    virtual void SwapBuffers() = 0;
};

} // namespace hazel
