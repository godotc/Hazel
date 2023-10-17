#pragma once

#include "hazel/hazel.h"

#ifdef HAZEL_LINKAGE_PROGRAM

extern hazel::App *hazel::CreateApplication();

int main()
{
    std::cout << "The HZ~ Engine" << std::endl;

    HZ_PROFILE_SESSION_BEGIN("Init", FPath("output/debug/hazel_profile-init.json"));
    hazel::App *app = hazel::CreateApplication();
    HZ_PROFILE_SESSION_END();

    HZ_PROFILE_SESSION_BEGIN("Runtime", FPath("output/debug/hazel_profile-runtime.json"));
    app->Run();
    HZ_PROFILE_SESSION_END();

    HZ_PROFILE_SESSION_BEGIN("ShutDown", FPath("output/debug/hazel_profile-showdown.json"));
    delete app;
    HZ_PROFILE_SESSION_END();

    return 0;
}



#endif