#include "hz_pch.h"

#include "hazel/core/entry_point.h"
#include <hazel.h>

#include <hazel/imgui/imgui_layer.h>


#include "platform/opengl/opengl_shader.h"
#include <hazel/core/app.h>

// entry point

#include "example_layer.h"
#include "sandbox_2d_layer.h"



class Sandbox : public hazel::App
{
  public:
    Sandbox()
    {
        HZ_INFO("Sandbox construct");
        // PushLayer(new hazel::ExampleLayer);
        auto sb = new Sandbox2D();
        PushLayer(sb);
        GetWindow().SetVSync(true);

        RESET_IMGUI_CONTEXT();
    }
    ~Sandbox() override = default;
};


hazel::App *hazel::CreateApplication(hazel::AppCommandLineArgs /*args*/)
{
    return new Sandbox;
}
