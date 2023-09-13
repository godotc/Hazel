#include <hazel.h>



class Sandbox : public hazel::App
{
  public:
    Sandbox()
    {
    }

    ~Sandbox()
    {
    }
};



hazel::App *hazel::CreateApplication()
{
    return new Sandbox();
}
