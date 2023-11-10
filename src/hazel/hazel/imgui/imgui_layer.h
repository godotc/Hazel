#pragma once

#include "hazel/core/layer.h"

#include "imgui.h"


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


    void SetBlockEvents(bool bBlock) { bBlockEvents = bBlock; }
};

} // namespace hazel
