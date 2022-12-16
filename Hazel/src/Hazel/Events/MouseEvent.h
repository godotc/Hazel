#pragma once

#include<sstream>

#include"Event.h"


namespace Hazel {


class HAZEL_API MouseMovedEvent : public Event
{
public:
	MouseMovedEvent(float x, float y) :
		m_MouseX(x), m_MouseY(y) {}

	std::string ToString() const override
	{
		std::stringstream ss;
		ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
		return ss.str();
	}

	EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryMouse)

private:
	float m_MouseX, m_MouseY;
};

class HAZEL_API MouseScrolledEvent : public Event
{
public:
	MouseScrolledEvent(float xOffset, float yOffset)
		:m_XOffset(xOffset), m_YOffset(yOffset) {}


	inline float GetXOffset() { return m_XOffset; }
	inline float GetYOffset() { return m_YOffset; }


	std::string ToString()const override
	{
		std::stringstream ss;
		ss << "MouseScrolledEvent: " << m_XOffset << ", " << m_YOffset;
		return ss.str();
	}

	EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryMouse)

private:
	float m_XOffset, m_YOffset;
};


class HAZEL_API MouseButtonEvent : public Event
{
public:
	MouseButtonEvent(int button)
		:m_Button(button) {}


	std::string ToString()const override
	{
		std::stringstream ss;
		ss << "MouseButtonEvent: " << m_Button;
		return ss.str();
	}

	EVENT_CLASS_TYPE(MouseButtonPressed)

protected:
	int m_Button;
};


class HAZEL_API MouseButtonReleasedEvent : public MouseButtonEvent
{
public:
	MouseButtonReleasedEvent(int button)
		:MouseButtonEvent(button) {}


	std::string ToString()const override
	{
		std::stringstream ss;
		ss << "MouseButtonReleasedEvent: " << m_Button;
		return ss.str();
	}

	EVENT_CLASS_TYPE(MouseButtonReleased)
};

}