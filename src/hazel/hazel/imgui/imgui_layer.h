#pragma once

#include "hazel/event/application_event.h"
#include "hazel/event/key_event.h"
#include "hazel/event/mouse_event.h"
#include "hazel/hazel/core/layer.h"
#include "imgui.h"


namespace hazel {

class HAZEL_API ImGuiLayer : public Layer
{
  public:
    ImGuiLayer();


  public:
    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnImGuiRender() override;

    void Begin();
    void End();

  private:
    float m_Time;
};

} // namespace hazel
