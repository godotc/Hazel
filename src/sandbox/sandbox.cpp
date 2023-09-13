#include <hazel.h>
#include <spdlog/fwd.h>
#include <spdlog/logger.h>



class Sandbox : public hazel::App
{
  public:
    Sandbox()
    {
        HZ_CORE_WARN("world");
        HZ_CORE_ERROR("hello");


        HZ_WARN("world");
        HZ_ERROR("hello");

        int a = 0;

        HZ_TRACE("{}", a);
    }

    ~Sandbox()
    {
    }
};



hazel::App *hazel::CreateApplication()
{

    return new Sandbox();
}
