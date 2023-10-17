#include "hz_pch.h"

#include "hazel/hazel/core/entry_point.h"
#include <hazel.h>

#include <hazel/imgui/imgui_layer.h>


#include "platform/opengl/opengl_shader.h"

// entry point

#include "example_layer.h"
#include "sandbox_2d_layer.h"


class Sandbox : public hazel::App
{
  public:
    Sandbox()
    {
        HZ_INFO("Sandbox construct");
        //        PushLayer(new hazel::ExampleLayer);
        PushLayer(new Sandbox2D());
        GetWindow().SetVSync(true);
    }
    ~Sandbox() override = default;
};


hazel::App *hazel::CreateApplication()
{
    return new Sandbox;
}
