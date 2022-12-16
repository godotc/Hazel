#pragma once

#ifdef  HZ_PLATFORM_WINDOWS

extern Hazel::Application* Hazel::CreateApplication();

int main(int argc, char** argv)
{
	Hazel::Log::Init();

#ifdef HZ_LOG_TEST
	//Hazel::Log::GetCoreLogger()->warn("warnning");
	//Hazel::Log::GetClientLogger()->error("error");
	HZ_INFO("info");
	HZ_CORE_ERROR("core error");
	int a = 5, b = 6;
	HZ_INFO("a={0}", a);
	HZ_WARN("b={1},a={0}", a, b);
#endif

	auto app = Hazel::CreateApplication();
	app->Run();
	delete app;
	return 0;
}

#else
#error Hazel(cherno) Like shitting windows!
#endif //  HZ_PLATFORM_WINDOWS


