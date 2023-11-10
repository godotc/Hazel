//
// Created by nono on 9/16/23.
//
#pragma once

#include "base.h"

namespace hazel {

class HAZEL_API Input
{
  public:
    static bool                    IsKeyPressed(int keycode);
    static bool                    IsMouseButtonPressed(int button);
    static std::pair<float, float> GetMousePos();
    static float                   GetMouseX();
    static float                   GetMouseY();
};

} // namespace hazel
