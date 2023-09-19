//
// Created by nono on 9/18/23.
//

#pragma once

namespace hazel {

class GraphicsContext
{
  public:
    virtual void Init()        = 0;
    virtual void SwapBuffers() = 0;
};

} // namespace hazel
