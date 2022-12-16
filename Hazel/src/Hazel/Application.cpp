#include "Application.h"

#include"Hazel/Events/ApplicationEvent.h"
#include"Hazel/Log.h"

namespace Hazel {

Application::Application()
{
}
Application::~Application()
{
}

void Application::Run()
{
#ifdef  HZ_EVENT_TEST
	WindowsResizeEvent e(800, 600);
	if (e.IsInCategory(EventCategoryApplication))
	{
		HZ_INFO(e);
	}
	if (e.IsInCategory(EventCategoryInput))
	{
		HZ_INFO(e);
	}
#endif //  HZ_EVENT_TEST


	while (true)
	{
	}
}

}
