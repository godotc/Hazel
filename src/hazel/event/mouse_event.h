#pragma once



#include "__microshit_api_hazel.h"
#include "fmt/format.h"
#include "hazel/event/event.h"
#include <algorithm>
#include <string>


namespace hazel {


class HAZEL_API MouseMoveEvent : public Event
{
  public:
    MouseMoveEvent(float x, float y) : m_MouseX(x), m_MouseY(y) {}

    std::string ToString() const override { return fmt::format("MouseMovedEvent: {}, {} ", m_MouseX, m_MouseY); }

    EVENT_CLASS_TYPE(MouseMoved)
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput);

  private:
    float m_MouseX, m_MouseY;
};



class HAZEL_API MouseButtonEvent : public Event
{
  public:
    inline int GetMouseButton() const { return m_Button; }


    EVENT_CLASS_CATEGORY(EventCategoryMouseButton | EventCategoryInput)

  protected:
    MouseButtonEvent(int button) : m_Button(button){};
    int m_Button;
};

class HAZEL_API MousePressedEvent : public MouseButtonEvent
{
  public:
    MousePressedEvent(int keycode) : MouseButtonEvent(keycode){};

    std::string ToString() const override { return fmt::format("MousePressedEvent: {} ", m_Button); }

    EVENT_CLASS_TYPE(MouseButtonPressed)
};


class HAZEL_API MouseReleasedEvent : public MouseButtonEvent
{
  public:
    MouseReleasedEvent(int keycode) : MouseButtonEvent(keycode){};

    std::string ToString() const override { return fmt::format("MouseReleasedEvent: {} ", m_Button); }

    EVENT_CLASS_TYPE(MouseButtonReleased)
};

} // namespace hazel