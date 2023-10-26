//
// Created by nono on 9/16/23.
//
#pragma once

#include "hazel/core/input.h"

namespace hazel {

class LinuxInput : public Input
{
  protected:
    bool                    IsKeyPressedImpl(int keycode) override;
    bool                    IsMouseButtonPressedImpl(int button) override;
    std::pair<float, float> GetMousePosImpl() override;
    float                   GetMouseXImpl() override;
    float                   GetMouseYImpl() override;
};

} // namespace hazel
