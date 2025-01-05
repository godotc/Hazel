//
/*
 * @ Author: godot42
 * @ Create Time: 2025-01-05 01:32:51
 * @ Modified by: @godot42
 * @ Modified time: 2025-01-06 02:31:59
 * @ Description:
 */

#pragma once



#ifdef _WIN32
    #ifdef TYPE_BUILD_SHARED
        #define NELUA_API __declspec(dllexport)
    #else
        #define NELUA_API __declspec(dllimport)
    #endif
#else
    #define NELUA_API
#endif
