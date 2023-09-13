#pragma once

#include <pthread.h>
namespace hazel {


class App
{
  public:
    App();
    virtual ~App();

  public:
    void Run();
};


App *CreateApplication();


} // namespace hazel
