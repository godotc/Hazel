#pragma once


#ifdef HZ_PLATFORM_WINDOWS
	#ifdef HZ_BUILD_DLL
		#define HAZEL_API __declspec(dllexport)
	#else
		#define HAZEL_API __declspec(dllimport)
	#endif
#else
	#error Hazel(cherno) Like shitting windows!
#endif // HZ_PLATFORM_WINDOWS


#define BIT(x) (1<<x)

