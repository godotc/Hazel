#pragma once


#include "__microshit_api_hazel.h"
#include "hazel/event/event.h"
#include <string>


namespace hazel {


class HAZEL_API KeyEvent : public Event
{
  public:
    inline int GetKeyCode() const { return m_KeyCode; }

    EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput);

  protected:
    KeyEvent(int key_code) : m_KeyCode(key_code) {}
    int m_KeyCode;
};



class HAZEL_API KeyPressedEvent : public KeyEvent
{
  public:
    KeyPressedEvent(int keycode, int repeat_count) : KeyEvent(keycode), m_RepeatCount(repeat_count) {}

    [[nodiscard]] std::string to_string() const override { return fmt::format("KeyPressedEvent: {} ({} repeats)", m_KeyCode, m_RepeatCount); }
    int                       GetRepeatCount() const { return m_RepeatCount; }

    EVENT_CLASS_TYPE(KeyPressed)

  protected:
    int m_RepeatCount;
};

class HAZEL_API KeyReleasedEvent : public KeyEvent
{
  public:
    explicit KeyReleasedEvent(int keycode) : KeyEvent(keycode) {};

    [[nodiscard]] std::string to_string() const override { return fmt::format("KeyReleasedEvent: {} ", m_KeyCode); }

    EVENT_CLASS_TYPE(KeyReleased)

  protected:
};



class HAZEL_API KeyTypedEvent : public KeyEvent
{
  public:
    explicit KeyTypedEvent(int keycode) : KeyEvent(keycode) {}

    [[nodiscard]] std::string to_string() const override { return fmt::format("KeyTypeEvent: {} ", m_KeyCode); }

    EVENT_CLASS_TYPE(KeyTyped)
};


} // namespace hazel