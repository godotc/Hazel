#pragma once

#include "hz_pch.h"

#include "hazel/event/event.h"
#include "hazel/core/base.h"


namespace hazel {


class HAZEL_API WindowResizeEvent : public Event
{
    using uint = unsigned int;

  public:
    WindowResizeEvent(uint w, uint h) : m_Height(h), m_Width(w) {}


    inline uint GetWidth() const { return m_Width; }
    inline uint GetHeight() const { return m_Height; }

    [[nodiscard]] std::string to_string() const override { return fmt::format("WindowResizeEvent: {}, {}", m_Width, m_Height); }


    EVENT_CLASS_CATEGORY(EventCategoryApplication)
    EVENT_CLASS_TYPE(WindowResize)

  private:
    uint m_Height, m_Width;
};


class HAZEL_API WindowCloseEvent : public Event
{
  public:
    WindowCloseEvent(){};


    std::string to_string() { return fmt::format("WindowCloseEvent"); }
    //

    EVENT_CLASS_CATEGORY(EventCategoryApplication)
    EVENT_CLASS_TYPE(WindowClose)
};

class HAZEL_API AppTickEvent : public Event
{
  public:
    AppTickEvent() {}


    std::string to_string() { return fmt::format("AppTickEvent"); }


    EVENT_CLASS_CATEGORY(EventCategoryApplication)
    EVENT_CLASS_TYPE(AppTick)
};

class HAZEL_API AppUpdateEvent : public Event
{
  public:
    AppUpdateEvent(){};


    std::string to_string() { return fmt::format("AppUpdateEvent"); }


    EVENT_CLASS_CATEGORY(EventCategoryApplication)
    EVENT_CLASS_TYPE(AppUpdate)
};

class HAZEL_API AppRenderEvent : public Event
{
  public:
    AppRenderEvent(){};


    std::string to_string() { return fmt::format("AppRenderEvent"); }


    EVENT_CLASS_CATEGORY(EventCategoryApplication)
    EVENT_CLASS_TYPE(AppRender)
};


} // namespace hazel