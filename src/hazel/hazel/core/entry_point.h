#pragma once

#include "hazel/hazel.h"

#ifdef HAZEL_LINKAGE_PROGRAM

extern hazel::App *hazel::CreateApplication();

int main()
{
    std::cout << "The HZ~ Engine" << std::endl;
    hazel::App *app = hazel::CreateApplication();
    app->Run();
    delete app;

    return 0;
}



#endif