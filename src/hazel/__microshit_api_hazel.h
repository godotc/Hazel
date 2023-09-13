

#pragma once

#ifdef _WIN32
	#ifdef BUILD_SHARED_HAZEL
		#define HAZEL_API __declspec(dllexport)
	#else
		#define HAZEL_API __declspec(dllimport)
	#endif
#else
	#define HAZEL_API extern
#endif
