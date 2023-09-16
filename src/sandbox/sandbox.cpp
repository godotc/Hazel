#include <hazel.h>
#include <hazel/imgui/imgui_layer.h>
#include <ostream>


class NothingLayer : public hazel::Layer
{
  public:
    NothingLayer() : Layer("nothing") {}

    void OnUpdate() override
    {
        // HZ_INFO("{}::Update ", GetName());
    }

    void OnEvent(hazel::Event &event) override
    {
        // HZ_TRACE("{}::OnEvent", GetName());
    }
};


class Sandbox : public hazel::App
{
  public:
    Sandbox()
    {
        HZ_INFO("Sandbox construct");
        PushLayer(new NothingLayer);
        PushLayer(new hazel::ImGuiLayer);
    }

    ~Sandbox()
    {
    }
};



hazel::App *hazel::CreateApplication()
{
    return new Sandbox;
}
