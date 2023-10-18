

#pragma once

#ifdef _WIN32
    #ifdef BUILD_SHARED_HAZEL
        #define HZ_UTILS_API __declspec(dllexport)
    #else
        #define HZ_UTILS_API __declspec(dllimport)
    #endif
#else
    #define HZ_UTILS_API
#endif


#define UTILS_API HZ_UTILS_API