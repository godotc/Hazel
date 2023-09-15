#pragma once



#include "__microshit_api_hazel.h"
#include "fmt/format.h"
#include "hazel/event/event.h"
#include <algorithm>
#include <sstream>
#include <string>


namespace hazel {


class HAZEL_API MouseMoveEvent : public Event
{
  public:
    MouseMoveEvent(float x, float y) : m_MouseX(x), m_MouseY(y) {}

    std::string to_string() const override { return fmt::format("MouseMovedEvent: {}, {} ", m_MouseX, m_MouseY); }

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

class HAZEL_API MouseButtonPressedEvent : public MouseButtonEvent
{
  public:
    MouseButtonPressedEvent(int keycode) : MouseButtonEvent(keycode){};

    std::string to_string() const override { return fmt::format("MousePressedEvent: {} ", m_Button); }

    EVENT_CLASS_TYPE(MouseButtonPressed)
};


class HAZEL_API MouseButtonReleasedEvent : public MouseButtonEvent
{
  public:
    MouseButtonReleasedEvent(int keycode) : MouseButtonEvent(keycode){};

    std::string to_string() const override { return fmt::format("MouseReleasedEvent: {} ", m_Button); }

    EVENT_CLASS_TYPE(MouseButtonReleased)
};

} // namespace hazel