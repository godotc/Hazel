#pragma once

#include "__microshit_api_hazel.h"
#include <cstdint>
#include <functional>
#include <ostream>
#include <stdint.h>


#include <hazel/core.h>
#include <string>
#include <type_traits>

namespace hazel {


enum class EventType
{

    None = 0,

    WindowClose,
    WindowResize,
    WindowFocus,
    WindowLostFocus,
    WindowMoved,

    AppTick,
    AppUpdate,
    AppRender,

    KeyPressed,
    KeyReleased,

    MouseMoved,
    MouseScrolled,
    MouseButtonPressed,
    MouseButtonReleased,

    EventTypeCount,
};


enum EventCategory : uint32_t
{
    None                     = 0,
    EventCategoryApplication = BIT(0),
    EventCategoryInput       = BIT(1),
    EventCategoryKeyboard    = BIT(2),
    EventCategoryMouse       = BIT(3),
    EventCategoryMouseButton = BIT(4)
};


#define EVENT_CLASS_TYPE(type)                                                    \
    static EventType    GetStaticType() { return EventType::type; }               \
    virtual EventType   GetEventType() const override { return GetStaticType(); } \
    virtual const char *GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) \
    virtual int GetCategoryFlags() const override { return category; }

class EventDispatcher;

class HAZEL_API Event
{
    friend class EventDispatcher;

  public:
    virtual EventType   GetEventType() const     = 0;
    virtual const char *GetName() const          = 0;
    virtual int         GetCategoryFlags() const = 0;
    virtual std::string ToString() const { return GetName(); }

    inline bool IsInCategory(EventCategory category)
    {
        return GetCategoryFlags() & category;
    }

  protected:
    bool bHandled = false;
};

class EventDispatcher
{
    template <class T>
    using EventFn = std::function<bool(T &)>;

  public:

    EventDispatcher(Event &ev) : m_Event(ev) {}

    template <class T>
    bool Dispatch(EventFn<T> func)
    {
        if (m_Event.GetEventType() == T::GetStaticType)
        {
            m_Event.bHandled = func(*(T *)&m_Event);
            return true;
        }
        return false;
    }

  private:
    Event &m_Event;
};


inline std::ostream &operator<<(std::ostream &os, const Event &ev) { return os << ev.ToString(); }


} // namespace hazel