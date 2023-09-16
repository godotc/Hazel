#pragma once

#include "hazel/event/application_event.h"
#include "hazel/event/key_event.h"
#include "hazel/event/mouse_event.h"
#include "hazel/layer.h"

namespace hazel {

class HAZEL_API ImGuiLayer : public Layer
{
  public:
    ImGuiLayer();


  public:
    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate() override;
    void OnEvent(Event &event) override;


  private:
    bool OnKeyTypedEvent(KeyTypedEvent &ev);

    bool OnKeyPressedEvent(KeyPressedEvent &ev);
    bool OnKeyReleasedEvent(KeyReleasedEvent &ev);

    bool OnMouseButtonPressedEvent(MouseButtonPressedEvent &ev);
    bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent &ev);
    bool OnMouseScrolledEvent(MouseScrolledEvent &ev);
    bool OnMouseMoveEvent(MouseMoveEvent &ev);

    bool OnWindowResizedEvent(WindowResizeEvent &ev);

  private:
    float m_Time;
};

} // namespace hazel
