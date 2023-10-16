//
// Created by nono on 9/16/23.
//
#pragma once

#include "core.h"

namespace hazel {

class HAZEL_API Input
{
  public:
    static Input *Get();

  public:
    inline static bool                    IsKeyPressed(int keycode) { return Get()->IsKeyPressedImpl(keycode); }
    inline static bool                    IsMouseButtonPressed(int button) { return Get()->IsMouseButtonPressedImpl(button); }
    inline static std::pair<float, float> GetMousePos() { return Get()->GetMousePosImpl(); }
    inline static float                   GetMouseX() { return Get()->GetMouseXImpl(); }
    inline static float                   GetMouseY() { return Get()->GetMouseYImpl(); }

  protected:
    virtual bool                    IsKeyPressedImpl(int keycode)        = 0;
    virtual bool                    IsMouseButtonPressedImpl(int button) = 0;
    virtual std::pair<float, float> GetMousePosImpl()                    = 0;
    virtual float                   GetMouseXImpl()                      = 0;
    virtual float                   GetMouseYImpl()                      = 0;

  protected:
    static Input *m_Instance;
};

} // namespace hazel
