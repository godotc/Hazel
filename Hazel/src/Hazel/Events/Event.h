#pragma once


#include <Hazel/Core.h>

#include<string>
#include<functional>

namespace Hazel {

// Currently blocking events
// envent occur -> dispatched immediately and must be deal right then
// Maybe futhre be buffer event in an event bus, be a batch evnet on a update of a stage

enum class EventType
{
	None = 0,
	WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
	AppTick, AppUpdate, AppRender,
	KeyPressed, KeyReleased,
	MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
};

enum EventCategory
{
	None = 0,
	EventCategoryApplication = BIT(0),
	EventCategoryInput = BIT(1),
	EventCategoryKeyBoard = BIT(2),
	EventCategoryMouse = BIT(3),
	EventCategoryMouseButton = BIT(4),
};

#define EVENT_CLASS_TYPE(type)  static EventType GetStaticType(){return EventType::##type; }\
								virtual EventType GetEventType() const override{ return GetStaticType();}\
								virtual const char* GetName() const override{ return #type;}

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }


class HAZEL_API
	Event
{
public:
	virtual ~Event() = default;


	virtual EventType GetEventType() const = 0;
	virtual const char* GetName() const = 0;
	virtual int GetCategoryFlags() const = 0;
	virtual std::string ToString() const { return GetName(); }

	bool IsInCategory(EventCategory category)
	{
		return GetCategoryFlags() & category;
	}

protected:
	bool Handled = false;
};

class HAZEL_API
	EventDispatcher
{
public:
	EventDispatcher(Event& event)
		: m_Event(event) { }

	// F will be deduced by the compiler
	template<typename T, typename F>
	bool Dispatch(const F& func)
	{
		if (m_Event.GetEventType() == T::GetStaticType())
		{
			m_Event.Handled |= func(static_cast<T&>(m_Event));
			return true;
		}
		return false;
	}
private:
	Event& m_Event;
};

inline std::ostream& operator<<(std::ostream& os, const Event& e)
{
	return os << e.ToString();
}




}
