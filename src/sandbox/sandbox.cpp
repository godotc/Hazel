#include <hazel.h>
#include <ostream>
#include <spdlog/fwd.h>
#include <spdlog/logger.h>

class NothingLayer : public hazel::Layer
{
  public:
    NothingLayer() : Layer("nothing") {}

    void OnUpdate() override
    {
        HZ_INFO("{}::Update ", GetName());
    }

    void OnEvent(hazel::Event &event) override
    {
        HZ_TRACE("{}::OnEvent", GetName());
    }
};


class Sandbox : public hazel::App
{
  public:
    Sandbox()
    {
        PushLayer(new NothingLayer);
    }

    ~Sandbox()
    {
    }
};



hazel::App *hazel::CreateApplication()
{

    return new Sandbox();
}
