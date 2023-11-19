#pragma once

#include "hazel/core/layer.h"

#include "imgui.h"
#include "utils/delegate.h"
#include <cstddef>


namespace hazel {

class HAZEL_API ImGuiLayer : public Layer
{
    bool bBlockEvents = true;

  public:
    ImGuiLayer();

  public:
    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnImGuiRender() override{};
    void         OnEvent(Event &event) override;

    void Begin();
    void End();

    // Reset the ctx when cross dll bound on micro shit
    void ResetTheImguiContex();


    void SetBlockEvents(bool bBlock) { bBlockEvents = bBlock; }
};

} // namespace hazel
